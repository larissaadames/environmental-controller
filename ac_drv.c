#include "ac_drv.h"
#include "uart_drv.h"
#include "led_drv.h"

static bool s_on = false;

void ac_init(void)
{
    s_on = false;
}

void ac_on(void)
{
    s_on = true;
    uart_send_string("Ar-condicionado ligado\r\n");
    led_on(LED_BLUE);
}

void ac_off(void)
{
    s_on = false;
    uart_send_string("Ar-condicionado desligado\r\n");
    led_off(LED_BLUE);
}

bool ac_is_on(void)
{
    return s_on;
}
