#include "lamp_drv.h"
#include "uart_drv.h"
#include "led_drv.h"

#define LAMP_THRESHOLD_DEFAULT 3000U // 30 lux
#define LAMP_LED_TICKS 5U            // 5 x 1s = 5s

static bool s_on = false;
static uint32_t s_threshold = LAMP_THRESHOLD_DEFAULT;
static uint16_t s_led_ticks = 0;

void lamp_init(void)
{
    s_on = false;
    s_threshold = LAMP_THRESHOLD_DEFAULT;
}

void lamp_toggle(void)
{
    s_on = !s_on;
    if (s_on)
    {
        uart_send_string("Lampada acessa\r\n");
    }
    else
    {
        uart_send_string("Lampada apagada\r\n");
    }
    led_on(LED_GREEN);
    s_led_ticks = LAMP_LED_TICKS;
}

void lamp_tick(void)
{
    if (s_led_ticks == 0)
    {
        return;
    }
    s_led_ticks--;
    if (s_led_ticks == 0)
    {
        led_off(LED_GREEN);
    }
}

bool lamp_is_on(void)
{
    return s_on;
}

uint32_t lamp_get_threshold(void)
{
    return s_threshold;
}

void lamp_set_threshold(uint32_t centilux)
{
    s_threshold = centilux;
}
