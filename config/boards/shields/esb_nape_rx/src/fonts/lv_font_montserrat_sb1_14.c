/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: --font fonts-src/Montserrat-SemiBold.ttf --size 14 --bpp 1 --format lvgl --no-compress --no-prefilter --force-fast-kern-format --lv-include lvgl.h -r 0x20-0x7F -r 0xB0 -r 0x2022 --lv-font-name lv_font_montserrat_sb1_14 -o out/lv_font_montserrat_sb1_14.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_FONT_MONTSERRAT_SB1_14
#define LV_FONT_MONTSERRAT_SB1_14 1
#endif

#if LV_FONT_MONTSERRAT_SB1_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xfc, 0x30,

    /* U+0022 "\"" */
    0xff, 0xff,

    /* U+0023 "#" */
    0x33, 0x19, 0xbf, 0xe4, 0x42, 0x21, 0x13, 0xfe,
    0xcc, 0x66, 0x33, 0x0,

    /* U+0024 "$" */
    0x10, 0x10, 0x7e, 0xfe, 0xd0, 0xd0, 0x78, 0x3e,
    0x17, 0x93, 0xff, 0x7c, 0x10, 0x10,

    /* U+0025 "%" */
    0x71, 0x91, 0x32, 0x2c, 0x45, 0x7, 0x7c, 0x1c,
    0x42, 0x88, 0xd1, 0x32, 0x26, 0x38,

    /* U+0026 "&" */
    0x3c, 0x33, 0x19, 0x8f, 0x83, 0x83, 0xc7, 0x3f,
    0x8e, 0xc3, 0xbf, 0x40,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x26, 0x6c, 0xcc, 0xcc, 0xcc, 0xc6, 0x62,

    /* U+0029 ")" */
    0x46, 0x63, 0x33, 0x33, 0x33, 0x36, 0x64,

    /* U+002A "*" */
    0x27, 0xdd, 0xf2, 0x0,

    /* U+002B "+" */
    0x30, 0xcf, 0xcc, 0x30, 0xc0,

    /* U+002C "," */
    0xfe,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0xc, 0x21, 0x86, 0x10, 0xc3, 0x8, 0x61, 0x84,
    0x30, 0xc2, 0x0,

    /* U+0030 "0" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0x66, 0x3c,

    /* U+0031 "1" */
    0xf3, 0x33, 0x33, 0x33, 0x33,

    /* U+0032 "2" */
    0x7d, 0x8c, 0x18, 0x30, 0xe3, 0x8e, 0x18, 0x61,
    0xfc,

    /* U+0033 "3" */
    0xfe, 0x18, 0x60, 0xc3, 0xc7, 0xc1, 0x83, 0x8f,
    0xf0,

    /* U+0034 "4" */
    0xc, 0xc, 0x6, 0x6, 0x6, 0x66, 0x33, 0xfe,
    0xc, 0x6, 0x3, 0x0,

    /* U+0035 "5" */
    0x7e, 0x60, 0x60, 0x60, 0x7c, 0x7, 0x3, 0x3,
    0x47, 0x7c,

    /* U+0036 "6" */
    0x1e, 0x60, 0x40, 0xc0, 0xfe, 0xe7, 0xc3, 0xc3,
    0x67, 0x3c,

    /* U+0037 "7" */
    0xff, 0xc6, 0xc6, 0x4, 0xc, 0xc, 0x18, 0x18,
    0x18, 0x30,

    /* U+0038 "8" */
    0x7e, 0xc3, 0xc3, 0xc3, 0x3c, 0xe7, 0xc3, 0xc3,
    0xe7, 0x3c,

    /* U+0039 "9" */
    0x3c, 0xe6, 0xc3, 0xc3, 0xe7, 0x7f, 0x3, 0x2,
    0x6, 0x7c,

    /* U+003A ":" */
    0xc0, 0x3,

    /* U+003B ";" */
    0xc0, 0xfe,

    /* U+003C "<" */
    0x2, 0x1d, 0xe6, 0xe, 0x7, 0x81, 0x80,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0x1, 0xc1, 0xe0, 0x71, 0xee, 0x30, 0x0,

    /* U+003F "?" */
    0x7d, 0x8c, 0x18, 0x30, 0xe3, 0x86, 0xc, 0x0,
    0x30,

    /* U+0040 "@" */
    0xf, 0x81, 0xc3, 0x18, 0x4, 0xdf, 0xad, 0xdc,
    0xec, 0x67, 0x63, 0x3b, 0x19, 0xdd, 0xcb, 0x7f,
    0x98, 0x0, 0x71, 0x0, 0xf8, 0x0,

    /* U+0041 "A" */
    0xc, 0x1, 0xc0, 0x68, 0x9, 0x83, 0x30, 0x63,
    0x1f, 0xe3, 0x4, 0x40, 0xd8, 0x18,

    /* U+0042 "B" */
    0xfe, 0x61, 0xb0, 0xd8, 0x6f, 0xe6, 0x1f, 0x7,
    0x83, 0xc3, 0xff, 0x80,

    /* U+0043 "C" */
    0x1f, 0x38, 0xd8, 0x18, 0xc, 0x6, 0x3, 0x0,
    0xc0, 0x71, 0x8f, 0x80,

    /* U+0044 "D" */
    0xfe, 0x30, 0xec, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x6, 0xc3, 0xbf, 0x80,

    /* U+0045 "E" */
    0xff, 0x83, 0x6, 0xf, 0xf8, 0x30, 0x60, 0xc1,
    0xfc,

    /* U+0046 "F" */
    0xff, 0x83, 0x6, 0xc, 0x1f, 0xf0, 0x60, 0xc1,
    0x80,

    /* U+0047 "G" */
    0x1f, 0x38, 0xd8, 0x18, 0xc, 0x6, 0xf, 0x6,
    0xc3, 0x71, 0x8f, 0x80,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3f, 0xfe, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xc0,

    /* U+0049 "I" */
    0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x7e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0x46,
    0xf8,

    /* U+004B "K" */
    0xc3, 0x63, 0xb3, 0x99, 0x8d, 0x87, 0xe3, 0xb1,
    0x8c, 0xc3, 0x60, 0xc0,

    /* U+004C "L" */
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1,
    0xfc,

    /* U+004D "M" */
    0xc0, 0x7c, 0x1f, 0x83, 0xf8, 0xfd, 0x17, 0xb6,
    0xf2, 0x9e, 0x73, 0xc4, 0x78, 0xc,

    /* U+004E "N" */
    0xc1, 0xf0, 0xfc, 0x7e, 0x3d, 0x9e, 0x6f, 0x1f,
    0x8f, 0xc3, 0xe0, 0xc0,

    /* U+004F "O" */
    0x1f, 0xe, 0x39, 0x83, 0x60, 0x3c, 0x7, 0x80,
    0xf0, 0x1b, 0x6, 0x71, 0xc3, 0xe0,

    /* U+0050 "P" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc, 0xc0,
    0xc0, 0xc0,

    /* U+0051 "Q" */
    0x1f, 0xe, 0x31, 0x83, 0x60, 0x3c, 0x7, 0x80,
    0xf0, 0x1b, 0x6, 0x7b, 0xc3, 0xf0, 0x1c, 0x80,
    0xf0,

    /* U+0052 "R" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc, 0xc4,
    0xc6, 0xc3,

    /* U+0053 "S" */
    0x3e, 0xc2, 0xc0, 0xe0, 0xfc, 0x3f, 0x7, 0x3,
    0xc3, 0xfc,

    /* U+0054 "T" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0x63, 0x1f, 0x0,

    /* U+0056 "V" */
    0xc0, 0xd8, 0x36, 0x19, 0x86, 0x31, 0xc, 0xc1,
    0xb0, 0x78, 0xe, 0x3, 0x0,

    /* U+0057 "W" */
    0x41, 0x82, 0x61, 0x86, 0x63, 0xc6, 0x23, 0xc6,
    0x32, 0x4c, 0x36, 0x6c, 0x36, 0x6c, 0x1c, 0x38,
    0x1c, 0x38, 0x1c, 0x38,

    /* U+0058 "X" */
    0x61, 0x98, 0xc3, 0x30, 0x78, 0x1c, 0x7, 0x1,
    0xe0, 0xcc, 0x63, 0xb8, 0x60,

    /* U+0059 "Y" */
    0xc0, 0xd8, 0x63, 0x10, 0xcc, 0x1e, 0x7, 0x80,
    0xc0, 0x30, 0xc, 0x3, 0x0,

    /* U+005A "Z" */
    0xff, 0x7, 0x6, 0xc, 0x18, 0x18, 0x30, 0x60,
    0xe0, 0xff,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf,

    /* U+005C "\\" */
    0x40, 0xc1, 0x81, 0x3, 0x6, 0x4, 0x8, 0x18,
    0x10, 0x20, 0x60, 0x40, 0x80,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f,

    /* U+005E "^" */
    0x30, 0xc6, 0x92, 0x4f, 0x10,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0x63,

    /* U+0061 "a" */
    0x7c, 0x8c, 0x1b, 0xfc, 0x78, 0xf1, 0xbf,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xe6, 0xfc,

    /* U+0063 "c" */
    0x3e, 0xcf, 0x6, 0xc, 0x18, 0x19, 0x9f,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3f, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x67, 0x3f,

    /* U+0065 "e" */
    0x3c, 0x66, 0xc3, 0xff, 0xc0, 0xc0, 0x62, 0x3e,

    /* U+0066 "f" */
    0x39, 0x86, 0x3e, 0x61, 0x86, 0x18, 0x61, 0x86,
    0x0,

    /* U+0067 "g" */
    0x3f, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0x67, 0x3f,
    0x3, 0x46, 0x7c,

    /* U+0068 "h" */
    0xc1, 0x83, 0x7, 0xee, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x18,

    /* U+0069 "i" */
    0xc3, 0xff, 0xfc,

    /* U+006A "j" */
    0x30, 0x3, 0x33, 0x33, 0x33, 0x33, 0x3e,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xc6, 0xcc, 0xd8, 0xf8, 0xf8,
    0xcc, 0xc6, 0xc3,

    /* U+006C "l" */
    0xff, 0xff, 0xfc,

    /* U+006D "m" */
    0xfd, 0xee, 0x73, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63,

    /* U+006E "n" */
    0xfd, 0xcf, 0x1e, 0x3c, 0x78, 0xf1, 0xe3,

    /* U+006F "o" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,

    /* U+0070 "p" */
    0xfc, 0xe6, 0xc3, 0xc3, 0xc3, 0xc3, 0xe6, 0xfc,
    0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3f, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0x67, 0x3f,
    0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xff, 0x31, 0x8c, 0x63, 0x18,

    /* U+0073 "s" */
    0x7f, 0xc, 0x3e, 0x7c, 0x38, 0xfe,

    /* U+0074 "t" */
    0x61, 0x8f, 0x98, 0x61, 0x86, 0x18, 0x60, 0xe0,

    /* U+0075 "u" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf3, 0xbf,

    /* U+0076 "v" */
    0xc3, 0x43, 0x66, 0x66, 0x34, 0x3c, 0x1c, 0x18,

    /* U+0077 "w" */
    0xc2, 0x1a, 0x38, 0x99, 0xcc, 0xca, 0x62, 0xda,
    0x1c, 0xf0, 0xe3, 0x83, 0x18,

    /* U+0078 "x" */
    0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x66, 0xc3,

    /* U+0079 "y" */
    0xc3, 0x43, 0x62, 0x66, 0x24, 0x3c, 0x3c, 0x18,
    0x18, 0x90, 0xe0,

    /* U+007A "z" */
    0xfc, 0x31, 0x8c, 0x31, 0x8c, 0x3f,

    /* U+007B "{" */
    0x36, 0x66, 0x66, 0x6c, 0x66, 0x66, 0x63,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xc6, 0x66, 0x66, 0x63, 0x66, 0x66, 0x6c,

    /* U+007E "~" */
    0x65, 0x3a, 0x30,

    /* U+00B0 "°" */
    0x74, 0x63, 0x17, 0x0,

    /* U+2022 "•" */
    0xff, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 62, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 62, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 93, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 6, .adv_w = 159, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 141, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 32, .adv_w = 193, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 158, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 49, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 59, .adv_w = 78, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 66, .adv_w = 78, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 73, .adv_w = 93, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 77, .adv_w = 132, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 82, .adv_w = 55, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 83, .adv_w = 86, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 84, .adv_w = 55, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 83, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 96, .adv_w = 151, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 85, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 130, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 130, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 152, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 131, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 140, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 136, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 146, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 140, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 55, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 55, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 202, .adv_w = 132, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 205, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 212, .adv_w = 130, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 232, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 243, .adv_w = 168, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 170, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 161, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 281, .adv_w = 185, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 150, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 143, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 173, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 181, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 71, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 118, .box_w = 7, .box_h = 10, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 163, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 134, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 214, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 181, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 395, .adv_w = 189, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 163, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 189, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 436, .adv_w = 164, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 141, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 135, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 177, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 163, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 491, .adv_w = 256, .box_w = 16, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 155, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 148, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 149, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 547, .adv_w = 78, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 554, .adv_w = 83, .box_w = 7, .box_h = 14, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 567, .adv_w = 78, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 574, .adv_w = 132, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 579, .adv_w = 112, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 580, .adv_w = 134, .box_w = 4, .box_h = 2, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 581, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 599, .adv_w = 130, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 606, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 617, .adv_w = 139, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 625, .adv_w = 83, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 155, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 645, .adv_w = 153, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 65, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 66, .box_w = 4, .box_h = 14, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 665, .adv_w = 142, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 676, .adv_w = 65, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 679, .adv_w = 236, .box_w = 12, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 691, .adv_w = 153, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 706, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 717, .adv_w = 154, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 728, .adv_w = 94, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 733, .adv_w = 115, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 95, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 153, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 754, .adv_w = 129, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 762, .adv_w = 206, .box_w = 13, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 775, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 783, .adv_w = 129, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 794, .adv_w = 119, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 800, .adv_w = 83, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 807, .adv_w = 68, .box_w = 2, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 811, .adv_w = 83, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 818, .adv_w = 132, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 821, .adv_w = 94, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 825, .adv_w = 75, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 3}
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
    0, 0, 0, 2, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 10, 0, 6, -4,
    0, 4, 0, -12, -13, 1, 10, 4,
    3, -9, 1, 10, 0, 9, 2, 7,
    -3, 0, 13, 2, -1, 4, 0, -7,
    0, 0, 0, 0, -4, 3, 4, 0,
    0, -2, 0, -1, 2, 0, -2, 0,
    -2, -1, -4, 0, 0, -2, 0, -4,
    -3, 0, -6, 0, -28, 0, -4, -11,
    4, 7, 0, 0, -4, 2, 2, 7,
    4, -3, 4, 0, 0, -12, 0, 0,
    -8, 0, 0, -6, -3, -10, 0, -9,
    -1, 0, -7, 0, 0, 8, 0, -7,
    -2, 0, 0, 0, -3, 0, 0, -2,
    -16, 0, 0, -18, -2, 7, -10, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    8, 0, 2, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 9, 2,
    0, 0, 2, 4, 2, 7, -2, 0,
    4, -2, -8, -30, 1, 6, 4, 0,
    -3, 0, 7, 0, 7, 0, 7, 0,
    0, 0, 0, -2, 7, 0, 0, -3,
    -8, 0, 0, -2, 0, -2, 0, 1,
    -4, -3, -4, 1, 0, -2, 0, 0,
    0, -9, 1, 0, -15, 0, 0, 0,
    0, -12, 2, -15, 0, 0, -8, -2,
    0, 21, -3, -3, 2, 2, -1, 0,
    -3, 2, 0, 0, -12, -4, 0, -21,
    0, 2, -15, 0, 13, -4, 0, -8,
    8, 0, -16, -21, -16, -4, 7, 0,
    0, -15, 0, 2, -6, 0, -3, 0,
    -4, 0, 4, 7, -27, 10, 0, 1,
    0, 0, 0, 0, 1, 1, -3, -4,
    0, 0, 0, -2, 0, 0, -1, 0,
    0, 0, -4, 0, 0, -4, 0, -4,
    0, 0, 0, 0, 2, -1, 0, 0,
    -1, 2, 2, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, 0, 7, 0, 0, -2, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    -1, -1, 0, -2, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -7, -1, -7, 4, 0, -4, 2, 4,
    6, 0, -6, 0, -2, 0, 0, -10,
    2, -1, 2, -12, 2, 0, -11, 0,
    4, -7, 0, 0, 0, -2, 0, 0,
    -2, 0, 0, 0, 0, 0, -1, -1,
    0, -1, -3, 0, 0, 0, 0, 0,
    0, -2, 0, 0, -2, 0, -1, 0,
    -4, 2, 0, -2, 1, 2, 2, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 7, -1, 0, -7, 0, 6, -11,
    -11, -9, -4, 2, 0, -2, -15, -4,
    0, -4, 0, -4, 3, -4, 0, 2,
    1, -7, 2, 0, 0, 0, -2, 0,
    0, 1, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -7, 0, 0, 0, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 11,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, -2, 0, 0, -4, 0, 2,
    0, -1, 0, 0, 0, -3, 0, 2,
    0, -11, -7, 0, 0, 0, -3, -11,
    0, 0, -2, 2, 0, -9, 0, -3,
    0, 0, -4, 2, 0, -3, 0, 0,
    0, 2, 0, 1, -4, -4, 0, -2,
    -2, -3, 0, 0, 0, 0, 0, 0,
    -7, 0, 0, -4, 2, -7, 2, 0,
    2, 0, 0, 0, 2, 0, -1, 0,
    9, 0, 4, 0, 0, -3, 0, 4,
    0, 0, 0, 2, 0, 0, 7, 0,
    7, 0, 0, -13, 0, -2, 3, 7,
    -30, 0, 21, 2, -4, -4, 2, 2,
    -1, 0, -11, 0, 0, 11, -13, -4,
    0, -16, 9, 31, -13, 0, -1, 4,
    -6, 0, 0, -2, 0, 2, 28, -4,
    -2, 7, 6, -6, 2, 0, 0, 2,
    2, -3, -7, 0, -29, 7, 0, 0,
    0, 4, 4, 6, 0, 0, 7, 0,
    -15, -12, 0, 10, 7, 4, -9, 1,
    9, 0, 8, 0, 4, 2, 0, 12,
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
const lv_font_t lv_font_montserrat_sb1_14 = {
#else
lv_font_t lv_font_montserrat_sb1_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_MONTSERRAT_SB1_14*/

