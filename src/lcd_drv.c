#include "lcd_drv.h"
#include <driverlib.h>
#include <grlib.h>
#include "Crystalfontz128x128_ST7735.h"
#include "lamp_drv.h"
#include "log.h"
#include <string.h>

static Graphics_Context s_context;
static char s_last_line1[24] = "";
static char s_last_line2[24] = "";

// Applies the foreground/background pair for the given lamp state without
// touching whatever is already drawn on screen.
static void apply_theme(bool lights_on)
{
    if (lights_on)
    {
        // Luzes acesas: caracteres pretos em fundo branco
        Graphics_setForegroundColor(&s_context, GRAPHICS_COLOR_BLACK);
        Graphics_setBackgroundColor(&s_context, GRAPHICS_COLOR_WHITE);
    }
    else
    {
        // Luzes apagadas: caracteres brancos em fundo preto
        Graphics_setForegroundColor(&s_context, GRAPHICS_COLOR_WHITE);
        Graphics_setBackgroundColor(&s_context, GRAPHICS_COLOR_BLACK);
    }
}

// Redraws the last message with whatever theme is currently active.
static void redraw(void)
{
    Graphics_clearDisplay(&s_context);
    Graphics_drawStringCentered(&s_context, (int8_t *)s_last_line1,
                                AUTO_STRING_LENGTH, 64, 48, OPAQUE_TEXT);
    Graphics_drawStringCentered(&s_context, (int8_t *)s_last_line2,
                                AUTO_STRING_LENGTH, 64, 68, OPAQUE_TEXT);
}

void lcd_init(void)
{
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    Graphics_initContext(&s_context, &g_sCrystalfontz128x128);
    Graphics_setFont(&s_context, &g_sFontFixed6x8);

    apply_theme(lamp_is_on());
    Graphics_clearDisplay(&s_context);
    log_show_last();
}

void lcd_set_theme(bool lights_on)
{
    apply_theme(lights_on);
    redraw();
}

void lcd_show_message(const char *line1, const char *line2)
{
    strncpy(s_last_line1, line1 ? line1 : "", sizeof(s_last_line1) - 1);
    s_last_line1[sizeof(s_last_line1) - 1] = '\0';

    strncpy(s_last_line2, line2 ? line2 : "", sizeof(s_last_line2) - 1);
    s_last_line2[sizeof(s_last_line2) - 1] = '\0';

    redraw();
}
