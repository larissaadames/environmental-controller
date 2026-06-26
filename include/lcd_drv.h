#ifndef LCD_DRV_H_
#define LCD_DRV_H_

#include <stdbool.h>

void lcd_init(void);

void lcd_set_theme(bool lights_on);

void lcd_show_message(const char *line1, const char *line2);

void lcd_clear_screen(void);

#endif /* LCD_DRV_H_ */
