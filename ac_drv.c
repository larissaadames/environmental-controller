#include "ac_drv.h"
#include "uart_drv.h"
#include "led_drv.h"
#include "log.h"
#include "lcd_drv.h"

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
    log_event(LOG_AC_ON);
    lcd_show_message("Ar-condicionado", "LIGADO");
}

void ac_off(void)
{
    s_on = false;
    uart_send_string("Ar-condicionado desligado\r\n");
    led_off(LED_BLUE);
    log_event(LOG_AC_OFF);
    lcd_show_message("Ar-condicionado", "DESLIGADO");
}

bool ac_is_on(void)
{
    return s_on;
}
