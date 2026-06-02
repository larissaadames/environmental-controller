#include "ac_drv.h"
#include "uart_drv.h"

static int s_on = 0;

void ac_init(void)
{
    s_on = 0;
}

void ac_on(void)
{
    s_on = 1;
    uart_send_string("Ar-condicionado ligado\r\n");
    // TODO: Turn on blue led
    // TODO: Short beep
}

void ac_off(void)
{
    s_on = 0;
    uart_send_string("Ar-condicionado desligado\r\n");
    // TODO: Turn off blue led
    // TODO: Long beep
}

int ac_is_on(void)
{
    return s_on;
}
