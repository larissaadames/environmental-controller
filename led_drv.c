#include "led_drv.h"
#include "msp.h"

// Red = P2.6, Green = P2.4, Blue = P5.6 (BOOSTXL-EDUMKII)
typedef struct
{
    volatile uint8_t *out;
    uint8_t pin;
} led_pin_t;

static led_pin_t s_leds[3];

void led_init(void)
{
    P2->SEL0 &= ~(BIT6 | BIT4);
    P2->SEL1 &= ~(BIT6 | BIT4);
    P2->DIR |= (BIT6 | BIT4);
    P2->OUT &= ~(BIT6 | BIT4);

    P5->SEL0 &= ~BIT6;
    P5->SEL1 &= ~BIT6;
    P5->DIR |= BIT6;
    P5->OUT &= ~BIT6;

    s_leds[LED_RED] = (led_pin_t){&P2->OUT, BIT6};
    s_leds[LED_GREEN] = (led_pin_t){&P2->OUT, BIT4};
    s_leds[LED_BLUE] = (led_pin_t){&P5->OUT, BIT6};
}

void led_on(led_color_t color)
{
    *s_leds[color].out |= s_leds[color].pin;
}

void led_off(led_color_t color)
{
    *s_leds[color].out &= ~s_leds[color].pin;
}
