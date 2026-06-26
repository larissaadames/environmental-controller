#include "log.h"
#include "rtc_drv.h"
#include "uart_drv.h"
#include "driverlib.h"
#include "lcd_drv.h"
#include <string.h>

// ---------------------------------------------------------------------------
// Flash region: the top 4 sectors of bank 1 (0x3C000 - 0x3FFFF). They are
// contiguous, so the whole 16 KB log is one linear address range far from the
// program code (which lives at the start of flash). Records are written one at
// a time and the buffer wraps around: when a sector is reached again it is
// erased first, overwriting the oldest events.
//
// The program executes from bank 0, so erasing/programming bank 1 is allowed
// by the flash controller (it cannot operate on the bank it runs from).
// ---------------------------------------------------------------------------
#define LOG_BASE_ADDR 0x0003C000U
#define LOG_SECTOR_SIZE 4096U
#define LOG_NUM_SECTORS 4U
#define LOG_RECORD_SIZE 16U
#define LOG_RECORDS_PER_SECTOR (LOG_SECTOR_SIZE / LOG_RECORD_SIZE)   // 256
#define LOG_TOTAL_RECORDS (LOG_RECORDS_PER_SECTOR * LOG_NUM_SECTORS) // 1024

// An erased flash word reads as 0xFFFFFFFF, so a slot with this sequence number
// is empty (never written).
#define LOG_SEQ_EMPTY 0xFFFFFFFFU

// One log entry. Laid out as exactly 16 bytes so 256 records fit in each 4 KB
// sector with no record straddling a sector boundary.
typedef struct
{
    uint32_t seq; // monotonic counter; 0xFFFFFFFF marks an empty slot
    uint8_t type; // log_event_t
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t pad[4]; // pad to 16 bytes
} log_record_t;

// Sequence number of the next record to write. Because records are appended one
// per increment, the slot of any record is simply (seq % LOG_TOTAL_RECORDS).
static uint32_t s_next_seq;
static uint32_t s_write_index;

static uint32_t slot_addr(uint32_t index)
{
    return LOG_BASE_ADDR + index * LOG_RECORD_SIZE;
}

static const log_record_t *slot_ptr(uint32_t index)
{
    return (const log_record_t *)slot_addr(index);
}

void log_init(void)
{
    // Flash runs at 12 MHz, which needs zero wait states.
    FlashCtl_setWaitState(FLASH_BANK1, 0);

    // Allow erase/program on the 4 log sectors at the top of bank 1.
    FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,
                             FLASH_SECTOR28 | FLASH_SECTOR29 |
                                 FLASH_SECTOR30 | FLASH_SECTOR31);

    // Resume after a power cycle: scan every slot for the highest sequence
    // number written so far. The next record continues from there.
    uint32_t i;
    uint32_t max_seq = 0;
    bool found = false;

    for (i = 0; i < LOG_TOTAL_RECORDS; i++)
    {
        uint32_t seq = slot_ptr(i)->seq;
        if (seq != LOG_SEQ_EMPTY && (!found || seq > max_seq))
        {
            max_seq = seq;
            found = true;
        }
    }

    s_next_seq = found ? (max_seq + 1) : 0;
    s_write_index = s_next_seq % LOG_TOTAL_RECORDS;
}

// The same message that was sent over the UART when the event happened.
static const char *type_message(uint8_t type)
{
    switch (type)
    {
    case LOG_AC_ON:
        return "Ar-condicionado ligado";
    case LOG_AC_OFF:
        return "Ar-condicionado desligado";
    case LOG_LAMP_ON:
        return "Lampada acessa";
    case LOG_LAMP_OFF:
        return "Lampada apagada";
    default:
        return "Evento desconhecido";
    }
}

// Draw an event on the LCD, splitting its message into two centered lines at
// the first space (e.g. "Ar-condicionado desligado").
static void show_event_on_lcd(uint8_t type)
{
    const char *message = type_message(type);

    char buffer[256];
    strncpy(buffer, message, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *line1 = strtok(buffer, " ");
    char *line2 = strtok(NULL, " ");

    lcd_show_message(line1, line2);
}

void log_event(log_event_t type)
{
    rtc_datetime_t now = rtc_now();

    log_record_t rec;
    rec.seq = s_next_seq;
    rec.type = (uint8_t)type;
    rec.second = now.seconds;
    rec.minute = now.minutes;
    rec.hour = now.hours;
    rec.day = now.day;
    rec.month = now.month;
    rec.year = now.year;
    rec.pad[0] = 0xFF;
    rec.pad[1] = 0xFF;
    rec.pad[2] = 0xFF;
    rec.pad[3] = 0xFF;

    // Crossing into a new sector: erase it first so we overwrite the oldest
    // events (the circular wrap). Harmless on the first lap, when the sector is
    // already erased.
    if ((s_write_index % LOG_RECORDS_PER_SECTOR) == 0)
    {
        FlashCtl_eraseSector(slot_addr(s_write_index));
    }

    FlashCtl_programMemory(&rec, (void *)slot_addr(s_write_index), sizeof(rec));

    s_next_seq++;
    s_write_index = s_next_seq % LOG_TOTAL_RECORDS;

    show_event_on_lcd((uint8_t)type);
}

void log_show_last(void)
{
    uint32_t i;
    uint32_t max_seq = 0;
    uint32_t last_index = 0;
    bool found = false;

    for (i = 0; i < LOG_TOTAL_RECORDS; i++)
    {
        uint32_t seq = slot_ptr(i)->seq;
        if (seq != LOG_SEQ_EMPTY && (!found || seq > max_seq))
        {
            max_seq = seq;
            last_index = i;
            found = true;
        }
    }

    if (!found)
    {
        return;
    }

    show_event_on_lcd(slot_ptr(last_index)->type);
}

// Send a value as exactly two zero-padded decimal digits (00-99).
static void send_2digits(uint8_t value)
{
    uart_send_char((char)('0' + (value / 10U) % 10U));
    uart_send_char((char)('0' + value % 10U));
}

static void send_record(const log_record_t *r)
{
    uart_send_char('[');
    uart_send_uint(r->year);
    uart_send_char('-');
    send_2digits(r->month);
    uart_send_char('-');
    send_2digits(r->day);
    uart_send_char(' ');
    send_2digits(r->hour);
    uart_send_char(':');
    send_2digits(r->minute);
    uart_send_char(':');
    send_2digits(r->second);
    uart_send_string("] ");
    uart_send_string(type_message(r->type));
    uart_send_string("\r\n");
}

void log_dump(void)
{
    // Find the range of sequence numbers currently stored. After wrapping, the
    // oldest sector has been erased, so the valid records form the contiguous
    // range [min_seq, max_seq].
    uint32_t i;
    uint32_t min_seq = 0;
    uint32_t max_seq = 0;
    bool found = false;

    for (i = 0; i < LOG_TOTAL_RECORDS; i++)
    {
        uint32_t seq = slot_ptr(i)->seq;
        if (seq == LOG_SEQ_EMPTY)
        {
            continue;
        }

        if (!found)
        {
            min_seq = seq;
            max_seq = seq;
            found = true;
        }
        else if (seq < min_seq)
        {
            min_seq = seq;
        }
        else if (seq > max_seq)
        {
            max_seq = seq;
        }
    }

    if (!found)
    {
        uart_send_string("Nenhum log salvo\r\n");
        return;
    }

    // Walk the sequence numbers in order; the slot of each is (seq % total).
    uint32_t seq;
    uint32_t count = 0;
    for (seq = min_seq; seq <= max_seq; seq++)
    {
        const log_record_t *r = slot_ptr(seq % LOG_TOTAL_RECORDS);
        if (r->seq == LOG_SEQ_EMPTY)
        {
            continue; // defensive; should not happen for a contiguous range
        }
        send_record(r);
        count++;
    }

    uart_send_string("--- ");
    uart_send_uint((uint16_t)count);
    uart_send_string(" evento(s) ---\r\n");
}

void log_clear(void)
{
    uint32_t s;
    for (s = 0; s < LOG_NUM_SECTORS; s++)
    {
        FlashCtl_eraseSector(LOG_BASE_ADDR + s * LOG_SECTOR_SIZE);
    }

    s_next_seq = 0;
    s_write_index = 0;

    uart_send_string("OK: logs cleared\r\n");

    lcd_clear_screen();
}
