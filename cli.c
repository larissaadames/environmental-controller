#include "cli.h"
#include "queue.h"
#include "uart_drv.h"

#define RX_FIFO_SIZE 128
static volatile uint8_t s_fifo_data[RX_FIFO_SIZE];
static Queue s_rx;

// Line being assembled, owned by the main loop.
static char s_buf[CLI_BUFFER_SIZE];
static uint8_t s_len;
static int s_ready;

void cli_init(void)
{
    queue_init(&s_rx, s_fifo_data, RX_FIFO_SIZE);
}

void cli_rx_byte(char c)
{
    queue_push(&s_rx, (uint8_t)c);
}

int cli_poll(void)
{
    uint8_t byte;
    while (!s_ready && queue_peek(&s_rx, &byte))
    {
        queue_pop(&s_rx);
        char c = (char)byte;
        if (c == '\r' || c == '\n')
        {
            if (s_len == 0)
                continue; // skip empty lines (e.g. the LF of a CR+LF pair)
            uart_send_string("\r\n");
            s_buf[s_len] = '\0';
            s_ready = 1;
        }
        else
        {
            uart_send_char(c); // echo
            if (s_len < (CLI_BUFFER_SIZE - 1))
                s_buf[s_len++] = c;
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
