#include "led_drv.h"
#include "msp.h"

// Red = P2.6, Green = P2.4, Blue = P5.6 (BOOSTXL-EDUMKII)
typedef struct
{
    DIO_PORT_Odd_Interruptable_Type *port;
    uint8_t pin;
} led_pin_t;

static const led_pin_t s_leds[] = {
    [LED_RED] = {P2, BIT6},
    [LED_GREEN] = {P2, BIT4},
    [LED_BLUE] = {P5, BIT6},
};

void led_init(void)
{
	int i;
    for (i = 0; i < 3; i++)
    {
        s_leds[i].port->SEL0 &= ~s_leds[i].pin;
        s_leds[i].port->SEL1 &= ~s_leds[i].pin;
        s_leds[i].port->DIR |= s_leds[i].pin;
        s_leds[i].port->OUT &= ~s_leds[i].pin;
    }
}

void led_on(led_color_t color)
{
    s_leds[color].port->OUT |= s_leds[color].pin;
}

void led_off(led_color_t color)
{
    s_leds[color].port->OUT &= ~s_leds[color].pin;
}
