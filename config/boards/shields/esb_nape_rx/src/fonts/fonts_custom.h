/*
 * Custom-converted LVGL fonts for the Nape Deck UI.
 *
 * See README.md in this directory for the font source, license
 * and the exact lv_font_conv commands that produced the .c files
 * here.
 *
 * Why: the stock Zephyr/LVGL Montserrat (Medium, bpp 4) smears
 * visibly at every size on the Deck's ST7789V panel while drawn
 * shapes stay pixel-perfect (hardware bisect, 2026-11-04). These
 * are Montserrat SemiBold — heavier glyph strokes, so the
 * antialiased edge pixels carry less of the perceived shape —
 * converted at the sizes the UI uses (16/20/28), plus a Bold 20
 * that exists only as a comparison sample on the touchtest
 * pattern page 2. SemiBold 12/14 are also bundled as pattern
 * page 2 judging samples only — whether the heavier face makes
 * small sizes usable again is a hardware judgment (the user
 * expects 14 to be the floor); the production UI stays at
 * 16/20/28 until then. Compiled in only when
 * CONFIG_ESB_NAPE_RX_CUSTOM_FONT=y (see ../status_screen.c for
 * the FONT_16/20/28 routing macros).
 */
#ifndef ESB_NAPE_FONTS_CUSTOM_H
#define ESB_NAPE_FONTS_CUSTOM_H

#include <lvgl.h>

LV_FONT_DECLARE(lv_font_montserrat_sb_12);
LV_FONT_DECLARE(lv_font_montserrat_sb_14);
LV_FONT_DECLARE(lv_font_montserrat_sb_16);
LV_FONT_DECLARE(lv_font_montserrat_sb_20);
LV_FONT_DECLARE(lv_font_montserrat_sb_28);
LV_FONT_DECLARE(lv_font_montserrat_b_20);

#endif /* ESB_NAPE_FONTS_CUSTOM_H */
