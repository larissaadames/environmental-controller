#include "cli.h"
#include "uart_drv.h"

// RX byte FIFO. Single-producer (ISR advances s_head) / single-consumer (main
// advances s_tail) ring buffer, so no interrupt masking is needed.
#define RX_FIFO_SIZE 128
static volatile uint8_t s_fifo[RX_FIFO_SIZE];
static volatile uint8_t s_head;
static volatile uint8_t s_tail;

// Line being assembled, owned by the main loop.
static char s_buf[CLI_BUFFER_SIZE];
static uint8_t s_len;
static int s_ready;

void cli_rx_byte(char c)
{
    uint8_t next = (uint8_t)((s_head + 1) % RX_FIFO_SIZE);
    if (next != s_tail) // drop only if the FIFO is completely full
    {
        s_fifo[s_head] = (uint8_t)c;
        s_head = next;
    }
}

int cli_poll(void)
{
    while (!s_ready && s_head != s_tail)
    {
        char c = (char)s_fifo[s_tail];
        s_tail = (uint8_t)((s_tail + 1) % RX_FIFO_SIZE);

        if (c == '\r' || c == '\n')
        {
            if (s_len == 0)
            {
                continue; // skip empty lines (e.g. the LF of a CR+LF pair)
            }
            uart_send_string("\r\n");
            s_buf[s_len] = '\0';
            s_ready = 1;
        }
        else
        {
            uart_send_char(c); // echo
            if (s_len < (CLI_BUFFER_SIZE - 1))
            {
                s_buf[s_len++] = c;
            }
        }
    }
    return s_ready;
}

const char *cli_get_line(void)
{
    return s_buf;
}

void cli_clear(void)
{
    s_len = 0;
    s_ready = 0;
}
