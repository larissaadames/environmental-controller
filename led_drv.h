#ifndef LED_DRV_H_
#define LED_DRV_H_

typedef enum
{
    LED_RED = 0,
    LED_GREEN = 1,
    LED_BLUE = 2
} led_color_t;

// Configure LED pins as GPIO outputs, all off.
void led_init(void);

void led_on(led_color_t color);
void led_off(led_color_t color);

#endif /* LED_DRV_H_ */
