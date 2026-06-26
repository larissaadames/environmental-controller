#ifndef LCD_DRV_H_
#define LCD_DRV_H_

#include <stdbool.h>

typedef enum
{
    light_mode,
    dark_mode
} lcd_theme_t;

void lcd_init(lcd_theme_t initial_theme);

void lcd_set_theme(lcd_theme_t theme);

void lcd_show_message(const char *line1, const char *line2);

void lcd_clear_screen(void);

#endif /* LCD_DRV_H_ */
