/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --font fonts-src/Montserrat-SemiBold.ttf --size 16 --bpp 1 --format lvgl --no-compress --no-prefilter --force-fast-kern-format --lv-include lvgl.h -r 0x20-0x7F -r 0xB0 -r 0x2022 --lv-font-name lv_font_montserrat_sb1_16 -o out/lv_font_montserrat_sb1_16.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_FONT_MONTSERRAT_SB1_16
#define LV_FONT_MONTSERRAT_SB1_16 1
#endif

#if LV_FONT_MONTSERRAT_SB1_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xf,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xb0,

    /* U+0023 "#" */
    0x19, 0x83, 0x30, 0x66, 0x3f, 0xf7, 0xfe, 0x22,
    0x4, 0x47, 0xfe, 0xff, 0xc6, 0x60, 0xcc, 0x19,
    0x80,

    /* U+0024 "$" */
    0x18, 0x1f, 0xbf, 0xdb, 0x2d, 0x87, 0xc1, 0xf0,
    0x7e, 0x1b, 0x8c, 0xe6, 0x7f, 0xe7, 0xe0, 0xc0,
    0x60,

    /* U+0025 "%" */
    0x78, 0x46, 0x62, 0x33, 0x21, 0x9b, 0xc, 0xd0,
    0x3d, 0x0, 0xb, 0xc0, 0xb3, 0xd, 0x98, 0x4c,
    0xc4, 0x66, 0x21, 0xe0,

    /* U+0026 "&" */
    0x3e, 0xf, 0xe1, 0x8c, 0x31, 0x83, 0x60, 0x78,
    0x1f, 0x16, 0x3e, 0xc3, 0xd8, 0x79, 0xff, 0x9f,
    0x20,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x36, 0x6c, 0xcc, 0xcc, 0xcc, 0xcc, 0x66, 0x30,

    /* U+0029 ")" */
    0xc6, 0x63, 0x33, 0x33, 0x33, 0x33, 0x66, 0xc0,

    /* U+002A "*" */
    0x25, 0x7f, 0xfa, 0x90,

    /* U+002B "+" */
    0x18, 0x33, 0xff, 0xf1, 0x83, 0x6, 0x0,

    /* U+002C "," */
    0xf7, 0x80,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xfc,

    /* U+002F "/" */
    0x6, 0x18, 0x30, 0x61, 0x83, 0x6, 0x18, 0x30,
    0x61, 0x83, 0x6, 0x18, 0x30, 0x40,

    /* U+0030 "0" */
    0x1c, 0x3f, 0x98, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xb1, 0x9f, 0xc7, 0x80,

    /* U+0031 "1" */
    0xff, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x7c, 0xfe, 0xc7, 0x3, 0x3, 0x6, 0xe, 0x1c,
    0x38, 0x70, 0xff, 0xff,

    /* U+0033 "3" */
    0x7f, 0xbf, 0xc0, 0xc0, 0xc0, 0xc0, 0x78, 0x3e,
    0x3, 0x1, 0xa1, 0xff, 0xc7, 0xc0,

    /* U+0034 "4" */
    0x6, 0x3, 0x80, 0xc0, 0x60, 0x30, 0x1c, 0xc6,
    0x33, 0xff, 0xff, 0xc0, 0xc0, 0x30, 0xc,

    /* U+0035 "5" */
    0x3f, 0x1f, 0x8c, 0x6, 0x6, 0x3, 0xf1, 0xfc,
    0x7, 0x1, 0xa1, 0xff, 0xc7, 0xc0,

    /* U+0036 "6" */
    0x1f, 0x1f, 0x98, 0x18, 0xd, 0xe7, 0xfb, 0x8f,
    0x83, 0xc1, 0xb1, 0xdf, 0xc3, 0xc0,

    /* U+0037 "7" */
    0xff, 0xff, 0xf0, 0xd8, 0x60, 0x60, 0x30, 0x38,
    0x18, 0xc, 0xc, 0x6, 0x7, 0x0,

    /* U+0038 "8" */
    0x3e, 0x3f, 0xb8, 0xf8, 0x3e, 0x3b, 0xf9, 0xfd,
    0xc7, 0xc1, 0xf1, 0xdf, 0xc7, 0xc0,

    /* U+0039 "9" */
    0x3c, 0x3f, 0xb8, 0xd8, 0x3c, 0x1f, 0x1d, 0xfe,
    0x7b, 0x1, 0x81, 0x9f, 0x8f, 0x80,

    /* U+003A ":" */
    0xfc, 0xf, 0xc0,

    /* U+003B ";" */
    0xfc, 0x3, 0xde,

    /* U+003C "<" */
    0x1, 0xf, 0x3c, 0xf0, 0xe0, 0x78, 0xf, 0x3,

    /* U+003D "=" */
    0xff, 0xfc, 0x0, 0xf, 0xff, 0xc0,

    /* U+003E ">" */
    0x80, 0xe0, 0x7c, 0xf, 0x7, 0x3c, 0xf0, 0x80,

    /* U+003F "?" */
    0x3c, 0xff, 0xc7, 0x3, 0x3, 0x6, 0xc, 0x18,
    0x18, 0x0, 0x18, 0x18,

    /* U+0040 "@" */
    0x7, 0xc0, 0x3f, 0xe0, 0xe0, 0xe3, 0xbd, 0xe6,
    0xff, 0xdb, 0x8e, 0xf6, 0xd, 0xec, 0x1b, 0xd8,
    0x37, 0xb8, 0xed, 0xbf, 0xf3, 0xbc, 0xe3, 0x80,
    0x3, 0xfc, 0x1, 0xf0, 0x0,

    /* U+0041 "A" */
    0x6, 0x0, 0xf0, 0xf, 0x1, 0x90, 0x19, 0x81,
    0x98, 0x30, 0xc3, 0xfc, 0x7f, 0xe6, 0x6, 0x60,
    0x6c, 0x3,

    /* U+0042 "B" */
    0xfe, 0x3f, 0xec, 0x1b, 0x6, 0xc1, 0xbf, 0xcf,
    0xfb, 0x7, 0xc0, 0xf0, 0x7f, 0xfb, 0xfc,

    /* U+0043 "C" */
    0x1f, 0xf, 0xf7, 0xf, 0x80, 0xc0, 0x30, 0xc,
    0x3, 0x0, 0xe0, 0x1c, 0x33, 0xfc, 0x7c,

    /* U+0044 "D" */
    0xff, 0x1f, 0xf3, 0x7, 0x60, 0x7c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0xf8, 0x3b, 0xfe, 0x7f,
    0x80,

    /* U+0045 "E" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xff, 0xff, 0xc0,
    0xc0, 0xc0, 0xff, 0xff,

    /* U+0046 "F" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xff,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x1f, 0xf, 0xf7, 0xf, 0x80, 0xc0, 0x30, 0xc,
    0xf, 0x3, 0xe0, 0xdc, 0x33, 0xfc, 0x7c,

    /* U+0048 "H" */
    0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0, 0xff, 0xff,
    0xff, 0x3, 0xc0, 0xf0, 0x3c, 0xf, 0x3,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x7e, 0xfc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0x7,
    0x8f, 0xf3, 0xc0,

    /* U+004B "K" */
    0xc1, 0xb0, 0xec, 0x73, 0x38, 0xdc, 0x36, 0xf,
    0xc3, 0xb8, 0xc7, 0x30, 0xcc, 0x1b, 0x7,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xff, 0xff,

    /* U+004D "M" */
    0xc0, 0x3e, 0x7, 0xe0, 0x7f, 0xf, 0xf0, 0xfd,
    0x9b, 0xd9, 0xbc, 0xf3, 0xcf, 0x3c, 0x63, 0xc6,
    0x3c, 0x3,

    /* U+004E "N" */
    0xc0, 0xf8, 0x3f, 0xf, 0xc3, 0xf8, 0xf7, 0x3c,
    0xef, 0x1f, 0xc3, 0xf0, 0xfc, 0x1f, 0x3,

    /* U+004F "O" */
    0x1f, 0x83, 0xfc, 0x70, 0xee, 0x7, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xe0, 0x77, 0xe, 0x3f,
    0xc1, 0xf8,

    /* U+0050 "P" */
    0xfe, 0x7f, 0xb0, 0xf8, 0x3c, 0x1e, 0xf, 0xf,
    0xfe, 0xfe, 0x60, 0x30, 0x18, 0x0,

    /* U+0051 "Q" */
    0xf, 0x81, 0xfe, 0x1c, 0x38, 0xc0, 0xcc, 0x3,
    0x60, 0x1b, 0x0, 0xd8, 0x6, 0xe0, 0x73, 0x87,
    0x1f, 0xf0, 0x3f, 0x0, 0x71, 0x1, 0xf8, 0x3,
    0x80,

    /* U+0052 "R" */
    0xfe, 0x7f, 0xb0, 0xf8, 0x3c, 0x1e, 0xf, 0xf,
    0xfe, 0xfe, 0x63, 0x30, 0xd8, 0x30,

    /* U+0053 "S" */
    0x3e, 0x3f, 0xb8, 0x58, 0xe, 0x3, 0xe0, 0x7c,
    0x7, 0x1, 0xe1, 0xff, 0xc7, 0xc0,

    /* U+0054 "T" */
    0xff, 0xff, 0xf0, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+0055 "U" */
    0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xd8, 0x67, 0xf8, 0xfc,

    /* U+0056 "V" */
    0xc0, 0x76, 0x6, 0x60, 0x67, 0xc, 0x30, 0xc3,
    0x1c, 0x19, 0x81, 0x98, 0x1f, 0x0, 0xf0, 0xe,
    0x0, 0x60,

    /* U+0057 "W" */
    0x60, 0xc0, 0xd8, 0x38, 0x66, 0x1e, 0x18, 0xc7,
    0x86, 0x31, 0xb3, 0xc, 0x4c, 0xc3, 0xb3, 0x30,
    0x6c, 0x6c, 0x1b, 0x1e, 0x7, 0x87, 0x80, 0xe1,
    0xe0, 0x38, 0x30,

    /* U+0058 "X" */
    0x60, 0xce, 0x38, 0xc6, 0xd, 0x81, 0xf0, 0x1c,
    0x3, 0x80, 0xd8, 0x3b, 0x86, 0x31, 0xc3, 0x70,
    0x70,

    /* U+0059 "Y" */
    0xc0, 0xf8, 0x76, 0x18, 0xcc, 0x33, 0x7, 0x81,
    0xe0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0xff, 0xc0, 0xc0, 0xe0, 0xe0, 0x60, 0x60,
    0x70, 0x70, 0x30, 0x3f, 0xff, 0xf0,

    /* U+005B "[" */
    0xff, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf, 0xf0,

    /* U+005C "\\" */
    0xc0, 0x81, 0x83, 0x3, 0x6, 0xc, 0xc, 0x18,
    0x30, 0x30, 0x60, 0xc0, 0xc1, 0x83,

    /* U+005D "]" */
    0xff, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f, 0xf0,

    /* U+005E "^" */
    0x18, 0x70, 0xb3, 0x66, 0x48, 0xf0, 0x80,

    /* U+005F "_" */
    0xff, 0xff,

    /* U+0060 "`" */
    0xc6, 0x30,

    /* U+0061 "a" */
    0x7c, 0xfe, 0x3, 0x7f, 0xff, 0xc3, 0xc7, 0xff,
    0x7b,

    /* U+0062 "b" */
    0xc0, 0x60, 0x30, 0x1b, 0xcf, 0xf7, 0x1f, 0x7,
    0x83, 0xc1, 0xf1, 0xff, 0xdb, 0xc0,

    /* U+0063 "c" */
    0x3e, 0x7f, 0xe3, 0xc0, 0xc0, 0xc0, 0xe3, 0x7f,
    0x3e,

    /* U+0064 "d" */
    0x1, 0x80, 0xc0, 0x67, 0xb7, 0xff, 0x1f, 0x7,
    0x83, 0xc1, 0xf1, 0xdf, 0xe7, 0xb0,

    /* U+0065 "e" */
    0x3e, 0x3f, 0xb0, 0x7f, 0xff, 0xfe, 0x3, 0x84,
    0xfe, 0x1e, 0x0,

    /* U+0066 "f" */
    0x3c, 0xf9, 0x87, 0xef, 0xcc, 0x18, 0x30, 0x60,
    0xc1, 0x83, 0x0,

    /* U+0067 "g" */
    0x3d, 0xbf, 0xf8, 0xf8, 0x3c, 0x1e, 0xf, 0x8e,
    0xff, 0x3d, 0x81, 0xff, 0xc7, 0xc0,

    /* U+0068 "h" */
    0xc0, 0xc0, 0xc0, 0xde, 0xfe, 0xe7, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0069 "i" */
    0x30, 0xff, 0xff, 0xc0,

    /* U+006A "j" */
    0x0, 0xc0, 0x1, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8d, 0xee,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0x6c, 0x66, 0x63, 0x61,
    0xf8, 0xec, 0x63, 0x31, 0xd8, 0x60,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xdc, 0x73, 0xfb, 0xee, 0x79, 0xf0, 0xc3, 0xc3,
    0xf, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3, 0xc,

    /* U+006E "n" */
    0xde, 0xfe, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3,

    /* U+006F "o" */
    0x3e, 0x3f, 0xb8, 0xf8, 0x3c, 0x1e, 0xf, 0x8e,
    0xfe, 0x3e, 0x0,

    /* U+0070 "p" */
    0xde, 0x7f, 0xb8, 0xf8, 0x3c, 0x1e, 0xf, 0x8f,
    0xfe, 0xde, 0x60, 0x30, 0x18, 0x0,

    /* U+0071 "q" */
    0x3d, 0xbf, 0xf8, 0xf8, 0x3c, 0x1e, 0xf, 0x8e,
    0xff, 0x3d, 0x80, 0xc0, 0x60, 0x30,

    /* U+0072 "r" */
    0xdf, 0xf9, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x7d, 0xfb, 0x6, 0x7, 0xc0, 0xe1, 0xff, 0x7c,

    /* U+0074 "t" */
    0x60, 0xc3, 0xf7, 0xe6, 0xc, 0x18, 0x30, 0x60,
    0xf8, 0xf0,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7f,
    0x3b,

    /* U+0076 "v" */
    0xc1, 0xb0, 0xd8, 0xcc, 0x63, 0x21, 0xb0, 0x78,
    0x38, 0x1c, 0x0,

    /* U+0077 "w" */
    0xc3, 0x4, 0x87, 0x19, 0x8e, 0x33, 0x34, 0x42,
    0x6d, 0x86, 0x9b, 0xf, 0x14, 0xe, 0x38, 0x18,
    0x70,

    /* U+0078 "x" */
    0x63, 0xb9, 0x8d, 0x83, 0x81, 0xc0, 0xe0, 0xd8,
    0xc6, 0xe3, 0x80,

    /* U+0079 "y" */
    0xc1, 0xb0, 0xd8, 0xce, 0x63, 0x61, 0xb0, 0x78,
    0x38, 0xc, 0x4c, 0x3e, 0x1e, 0x0,

    /* U+007A "z" */
    0xff, 0xfc, 0x30, 0xc3, 0x86, 0x18, 0x7f, 0xfe,

    /* U+007B "{" */
    0x3b, 0xd8, 0xc6, 0x33, 0x9c, 0x63, 0x18, 0xc6,
    0x3c, 0xe0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xfc,

    /* U+007D "}" */
    0xe3, 0xc3, 0xc, 0x30, 0xc3, 0xcf, 0x30, 0xc3,
    0xc, 0x33, 0xce, 0x0,

    /* U+007E "~" */
    0x73, 0xff, 0xce,

    /* U+00B0 "°" */
    0x76, 0xe3, 0x1d, 0xb8,

    /* U+2022 "•" */
    0xff, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 71, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 71, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 106, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 7, .adv_w = 182, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 24, .adv_w = 161, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 41, .adv_w = 220, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 181, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 56, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 79, .adv_w = 89, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 87, .adv_w = 89, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 95, .adv_w = 106, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 99, .adv_w = 151, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 106, .adv_w = 62, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 108, .adv_w = 99, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 109, .adv_w = 62, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 110, .adv_w = 95, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 124, .adv_w = 172, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 98, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 174, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 150, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 161, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 156, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 167, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 243, .adv_w = 161, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 62, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 62, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 263, .adv_w = 151, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 271, .adv_w = 151, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 277, .adv_w = 151, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 285, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 265, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 326, .adv_w = 192, .box_w = 12, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 344, .adv_w = 195, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 184, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 211, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 172, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 163, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 198, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 207, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 82, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 135, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 187, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 153, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 244, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 504, .adv_w = 207, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 519, .adv_w = 216, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 186, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 216, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 576, .adv_w = 187, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 590, .adv_w = 161, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 154, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 202, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 187, .box_w = 12, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 652, .adv_w = 293, .box_w = 18, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 679, .adv_w = 177, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 696, .adv_w = 169, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 711, .adv_w = 170, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 725, .adv_w = 90, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 733, .adv_w = 95, .box_w = 7, .box_h = 16, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 747, .adv_w = 90, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 755, .adv_w = 151, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 762, .adv_w = 128, .box_w = 8, .box_h = 2, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 764, .adv_w = 154, .box_w = 4, .box_h = 3, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 766, .adv_w = 155, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 775, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 789, .adv_w = 149, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 798, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 812, .adv_w = 159, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 94, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 178, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 848, .adv_w = 175, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 860, .adv_w = 74, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 864, .adv_w = 76, .box_w = 5, .box_h = 16, .ofs_x = -2, .ofs_y = -3},
    {.bitmap_index = 874, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 888, .adv_w = 74, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 270, .box_w = 14, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 907, .adv_w = 175, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 916, .adv_w = 165, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 927, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 941, .adv_w = 176, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 955, .adv_w = 108, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 961, .adv_w = 132, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 969, .adv_w = 109, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 979, .adv_w = 174, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 988, .adv_w = 148, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 999, .adv_w = 235, .box_w = 15, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1016, .adv_w = 147, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1027, .adv_w = 148, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1041, .adv_w = 136, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1049, .adv_w = 95, .box_w = 5, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1059, .adv_w = 78, .box_w = 2, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1063, .adv_w = 95, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1075, .adv_w = 151, .box_w = 8, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 1078, .adv_w = 107, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 1082, .adv_w = 86, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x1f72
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 8051, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 1, 2, 0, 3, 4, 5,
    2, 6, 0, 7, 8, 9, 8, 9,
    10, 11, 0, 12, 13, 14, 15, 16,
    17, 18, 11, 19, 19, 0, 0, 0,
    20, 21, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 22, 23, 0, 0,
    24, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 22, 0, 0, 8,
    25, 8
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 3, 4, 5,
    2, 0, 6, 7, 8, 9, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 11,
    17, 18, 19, 20, 20, 0, 0, 0,
    21, 22, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 23, 24, 25, 0,
    26, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 25, 8,
    27, 8
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 3, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 12, 0, 6, -5,
    0, 5, 0, -14, -15, 1, 12, 5,
    4, -10, 1, 12, 0, 10, 3, 8,
    -4, 0, 15, 3, -1, 5, 0, -8,
    0, 0, 0, 0, -5, 4, 5, 0,
    0, -3, 0, -1, 3, 0, -3, 0,
    -3, -1, -5, 0, 0, -3, 0, -5,
    -4, 0, -6, 0, -31, 0, -5, -13,
    5, 8, 0, 0, -5, 3, 3, 8,
    5, -4, 5, 0, 0, -14, 0, 0,
    -9, 0, 0, -6, -3, -12, 0, -10,
    -1, 0, -8, 0, 0, 9, 0, -8,
    -3, 0, 0, 0, -4, 0, 0, -2,
    -18, 0, 0, -20, -3, 8, -12, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 3, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 10, 3,
    0, 0, 2, 5, 3, 8, -3, 0,
    5, -3, -9, -35, 1, 6, 5, 0,
    -3, 0, 8, 0, 8, 0, 8, 0,
    0, 0, 0, -3, 8, 0, 0, -4,
    -9, 0, 0, -3, 0, -2, 0, 1,
    -5, -4, -5, 1, 0, -3, 0, 0,
    0, -10, 1, 0, -17, 0, 0, 0,
    0, -14, 3, -17, 0, 0, -9, -2,
    0, 24, -3, -4, 3, 3, -1, 0,
    -4, 3, 0, 0, -14, -5, 0, -24,
    0, 3, -17, 0, 15, -5, 0, -9,
    9, 0, -18, -24, -18, -5, 8, 0,
    0, -17, 0, 3, -6, 0, -4, 0,
    -5, 0, 5, 8, -31, 12, 0, 1,
    0, 0, 0, 0, 1, 1, -3, -5,
    0, 0, 0, -3, 0, 0, -1, 0,
    0, 0, -5, 0, 0, -5, 0, -5,
    0, 0, 0, 0, 3, -1, 0, 0,
    -1, 3, 3, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 8, 0, 0, -3, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, 0, -3, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    -8, -1, -8, 5, 0, -5, 3, 5,
    6, 0, -6, 0, -3, 0, 0, -12,
    3, -1, 2, -14, 3, 0, -13, 0,
    5, -8, 0, 0, 0, -3, 0, 0,
    -3, 0, 0, 0, 0, 0, -1, -1,
    0, -1, -3, 0, 0, 0, 0, 0,
    0, -3, 0, 0, -3, 0, -1, 0,
    -5, 3, 0, -3, 1, 3, 3, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 8, -1, 0, -8, 0, 6, -13,
    -13, -10, -5, 3, 0, -2, -17, -5,
    0, -5, 0, -5, 4, -5, 0, 3,
    1, -8, 2, 0, 0, 0, -3, 0,
    0, 1, 0, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, 0, -8, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 13,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, -3, 0, 0, -5, 0, 3,
    0, -1, 0, 0, 0, -4, 0, 3,
    0, -13, -8, 0, 0, 0, -4, -13,
    0, 0, -3, 3, 0, -10, 0, -3,
    0, 0, -4, 3, 0, -4, 0, 0,
    0, 3, 0, 1, -5, -5, 0, -3,
    -3, -3, 0, 0, 0, 0, 0, 0,
    -8, 0, 0, -5, 3, -8, 3, 0,
    3, 0, 0, 0, 3, 0, -2, 0,
    10, 0, 5, 0, 0, -4, 0, 5,
    0, 0, 0, 3, 0, 0, 8, 0,
    8, 0, 0, -15, 0, -3, 4, 8,
    -35, 0, 24, 3, -5, -5, 3, 3,
    -1, 0, -13, 0, 0, 13, -15, -5,
    0, -18, 10, 36, -15, 0, -1, 5,
    -6, 0, 0, -3, 0, 3, 32, -5,
    -2, 8, 6, -6, 3, 0, 0, 3,
    3, -4, -8, 0, -33, 8, 0, 0,
    0, 5, 5, 6, 0, 0, 8, 0,
    -17, -14, 0, 12, 8, 4, -10, 1,
    10, 0, 9, 0, 5, 3, 0, 14,
    0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 25,
    .right_class_cnt     = 27,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t lv_font_montserrat_sb1_16 = {
#else
lv_font_t lv_font_montserrat_sb1_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_MONTSERRAT_SB1_16*/

