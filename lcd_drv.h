#ifndef LCD_DRV_H_
#define LCD_DRV_H_

#include <stdbool.h>

// Initialize the SPI bus, the Crystalfontz 128x128 controller and the GrLib
// graphics context. The initial color theme matches the current lamp state
// (see lamp_drv.h), so call this AFTER lamp_init().
void lcd_init(void);

// Sets the color theme used for every message drawn from now on:
//   lights_on == true  -> black text on a white background
//   lights_on == false -> white text on a black background
// Also repaints whatever message is currently on screen with the new theme,
// so the display flips immediately when the lamp toggles, even if no new
// message is shown.
void lcd_set_theme(bool lights_on);

// Clears the screen and draws up to two centered lines of text, using
// whatever theme was last set with lcd_set_theme().
void lcd_show_message(const char *line1, const char *line2);

#endif /* LCD_DRV_H_ */
