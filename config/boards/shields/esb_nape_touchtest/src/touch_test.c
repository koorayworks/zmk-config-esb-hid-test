/*
 * Touch bring-up test for the Nape Deck (esb_nape_touchtest only) — v4.
 *
 * v1–v3 all probed i2c0 (hidden pads P1.00/P1.10, then P1.02/P1.04,
 * polling, then driver-pulsed reset) and never saw a single ACK.
 * v4 stops guessing and copies the configuration that is proven on
 * this exact hardware — the user's prospector-bridge firmware:
 * the CST816T sits on &xiao_i2c, which is i2c1 (board-default pins
 * SDA=P0.04 / SCL=P0.05 = XIAO D4/D5), at 100 kHz, with the driver
 * in interrupt mode (irq on D0, reset on D1). See the shield
 * overlay for the devicetree side.
 *
 * This app only cross-checks and provides the tap target:
 *
 *  - ~4 s after boot, one address scan over the touch bus (the
 *    DT bus of the touch_sensor node — i2c1, not a hardcoded i2c0)
 *    logs every address that ACKs. If the driver found the chip,
 *    0x15 must show up here too; if the driver logged "failed
 *    reading chip id" but the scan ACKs, the chip is alive and the
 *    problem is narrower.
 *
 *  - A screen with a single explicitly styled button is loaded over
 *    the status screen; every press increments the on-screen
 *    counter and logs "TOUCHTEST tap #N", proving the chain
 *    CST816S -> input subsystem -> LVGL pointer input end to end.
 *    (The v1 button relied on the default theme and rendered
 *    invisible; the explicit styles stay.)
 *
 * v5 adds a static test-pattern screen (reached via the PATTERN
 * button on the tap screen, BACK returns) to bisect a "screen
 * looks smeared/blurry on a static image" report: a true 1 px
 * checkerboard on a canvas, 1 px lines, pure color bars, text in
 * every compiled font size, and a 1 px full-screen border frame.
 * If the 1 px features are crisp, panel + driver are fine and the
 * perceived smear lives in the status screen's rendering; if they
 * smear too, it is panel/driver level.
 *
 * v6 is not a new app version but the esb_nape_patternls variant
 * of this build (CONFIG_ESB_NAPE_TOUCHTEST_LANDSCAPE_PATTERN):
 * the panel is driven in the Prospector Scanner's geometry —
 * landscape 280x240, mdac 0x60, x-offset 20, via that shield's
 * &st7789v overrides — and the pattern screen is loaded first at
 * boot, with its vertical layout compacted to fit 240 rows. This
 * bisects scan direction: identical stack, identical pattern,
 * only the panel geometry changes. Without the config, every
 * code path below behaves exactly as v5.
 *
 * v7 adds a second pattern page (NEXT on the pattern page, BACK
 * returns to it) to discriminate WHY antialiased text smears.
 * Bisect rounds 1-4 exonerated the panel/driver/flush path: the
 * page-1 shapes (checkerboard, 1 px lines, color bars) render
 * pixel-perfect in both orientations, repaint guards / VDB size
 * / label colors changed nothing, and even Montserrat 20/28
 * smear (just less visibly than 12/14). Page 2 puts the
 * remaining suspects side by side on one static screen: unscii
 * bitmap-font text (1 bpp glyphs, no AA at all), Montserrat
 * control lines, solid-gray bar groups that mimic glyph strokes
 * at 25/50/75/100% (an AA edge pixel is exactly such a mid-gray
 * — but here drawn as a plain fill), white text blended over a
 * mid-gray fill, and a 16-step gray ramp. Crisp unscii + crisp
 * gray bars + smeared Montserrat would convict the AA
 * blend/glyph path itself; dirt in the gray bars/ramp would
 * point back at mid-tone reproduction.
 *
 * v8: the user took the custom-font route without waiting for
 * the page-2 verdict. When CONFIG_ESB_NAPE_RX_CUSTOM_FONT=y
 * (enabled by this shield's conf), page 2 gains sample lines
 * under the stock Montserrat controls — the custom-converted
 * Montserrat SemiBold the Deck UI now uses and a Bold for
 * comparison at 20 px, plus SemiBold at 14 and 12 to judge
 * whether the heavier face makes small sizes usable again
 * (expectation: 14 is the floor; the production UI stays at
 * 16/20/28 until judged). Page 2 lays out with a running y
 * cursor and slightly tighter section metrics than v7 so every
 * section stays inside the 240x240 common area with or without
 * the sample lines.
 *
 * The production status screen is not modified: esb_nape_rx still
 * creates it, and this screen is simply loaded over it once the
 * display is up (its refresh timer keeps updating the covered
 * screen, which is harmless for a bring-up build).
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <zmk/display.h>

LOG_MODULE_REGISTER(touchtest);

/* Custom-font comparison samples for pattern page 2 (v8). The
 * font objects are compiled by the esb_nape_rx shield library
 * (its src/fonts/), so they are only declared here — this file
 * must not reference them when the config is off. */
#if defined(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
LV_FONT_DECLARE(lv_font_montserrat_sb_12);
LV_FONT_DECLARE(lv_font_montserrat_sb_14);
LV_FONT_DECLARE(lv_font_montserrat_sb_20);
LV_FONT_DECLARE(lv_font_montserrat_b_20);
#endif

/* The bus that hosts the touch node (i2c1 via &xiao_i2c in v4) —
 * never hardcode an instance: v1–v3 scanned i2c0 while the chip
 * was on i2c1 all along. */
static const struct device *const i2c_bus = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(touch_sensor)));

/* ---- Hardware I2C scan (system work queue, ~4 s after boot) ---- */

static void scan_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!device_is_ready(i2c_bus)) {
        LOG_ERR("TOUCHTEST hwscan: touch bus not ready");
        return;
    }

    int found = 0;

    for (uint16_t addr = 0x08; addr <= 0x77; addr++) {
        uint8_t byte;

        /* A 1-byte read is the portable probe: devices that ACK
         * their address return success, everything else errors. */
        if (i2c_read(i2c_bus, &byte, 1, addr) == 0) {
            LOG_INF("TOUCHTEST hwscan: ACK at 0x%02x", addr);
            found++;
        }
    }

    LOG_INF("TOUCHTEST hwscan done: %d devices", found);
}

static K_WORK_DELAYABLE_DEFINE(scan_work, scan_work_handler);

/* ---- Screens (created once on the display work queue) ---- */

static lv_obj_t *tap_scr;
static lv_obj_t *pattern_scr;
static lv_obj_t *pattern2_scr;
static lv_obj_t *tap_label;
static unsigned int tap_count;

static void tap_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_PRESSED) {
        return;
    }

    tap_count++;
    lv_label_set_text_fmt(tap_label, "TAP TEST: %u", tap_count);
    LOG_INF("TOUCHTEST tap #%u", tap_count);
}

static void pattern_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
}

static void back_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    lv_scr_load(tap_scr);
    LOG_INF("TOUCHTEST tap screen loaded");
}

static void next_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    lv_scr_load(pattern2_scr);
    LOG_INF("TOUCHTEST pattern 2 screen loaded");
}

static void back2_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
}

/* A button with explicit styles: this build's default theme renders
 * a plain button (nearly) invisible, which made v1's screen look
 * dead. Returns the button; the caller adds a centered label. */
static lv_obj_t *styled_button(lv_obj_t *parent, uint32_t bg_hex, lv_event_cb_t cb) {
    lv_obj_t *btn = lv_button_create(parent);

    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_hex), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 12, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    return btn;
}

static void button_label(lv_obj_t *btn, const char *text, const lv_font_t *font) {
    lv_obj_t *label = lv_label_create(btn);

    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(label, text);
    lv_obj_center(label);
}

/* Plain filled rectangle with no theme influence (pattern pieces). */
static lv_obj_t *plain_rect(lv_obj_t *parent, int32_t x, int32_t y, int32_t w, int32_t h,
                            uint32_t color_hex) {
    lv_obj_t *o = lv_obj_create(parent);

    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(color_hex), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(o, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(o, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(o, 0, LV_PART_MAIN);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return o;
}

static void build_tap_screen(void) {
    tap_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(tap_scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(tap_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(tap_scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btn = lv_button_create(tap_scr);

    lv_obj_set_size(btn, 220, 120);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 24);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2266CC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 12, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, tap_btn_event_cb, LV_EVENT_PRESSED, NULL);

    tap_label = lv_label_create(btn);
    lv_obj_set_style_text_font(tap_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(tap_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_label_set_text(tap_label, "TAP TEST: 0");
    lv_obj_center(tap_label);

    lv_obj_t *pattern_btn = styled_button(tap_scr, 0x338833, pattern_btn_event_cb);

    lv_obj_set_size(pattern_btn, 220, 56);
    lv_obj_align(pattern_btn, LV_ALIGN_TOP_MID, 0, 168);
    button_label(pattern_btn, "PATTERN", &lv_font_montserrat_20);
}

#define CHECKER_SIZE 72

static lv_color_t checker_buf[CHECKER_SIZE * CHECKER_SIZE];

static void build_pattern_screen(void) {
    const int32_t W = lv_display_get_horizontal_resolution(NULL);
    const int32_t H = lv_display_get_vertical_resolution(NULL);

    pattern_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(pattern_scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(pattern_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(pattern_scr, LV_OBJ_FLAG_SCROLLABLE);

    /* 1 px white border frame around the whole screen: checks the
     * panel edges and the x/y offsets in one look. */
    lv_obj_t *frame = lv_obj_create(pattern_scr);

    lv_obj_set_pos(frame, 0, 0);
    lv_obj_set_size(frame, W, H);
    lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_color(frame, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_border_width(frame, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(frame, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(frame, 0, LV_PART_MAIN);
    lv_obj_clear_flag(frame, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    /* True 1 px checkerboard on a canvas, top-left. */
    for (int32_t y = 0; y < CHECKER_SIZE; y++) {
        for (int32_t x = 0; x < CHECKER_SIZE; x++) {
            checker_buf[y * CHECKER_SIZE + x] =
                ((x + y) & 1) ? lv_color_white() : lv_color_black();
        }
    }
    lv_obj_t *canvas = lv_canvas_create(pattern_scr);

    lv_canvas_set_buffer(canvas, checker_buf, CHECKER_SIZE, CHECKER_SIZE,
                         LV_COLOR_FORMAT_NATIVE);
    lv_obj_set_pos(canvas, 6, 6);
    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    /* 1 px lines: three verticals right of the checkerboard, three
     * horizontals below it. The sharpest possible test pattern. */
    for (int i = 0; i < 3; i++) {
        plain_rect(pattern_scr, W - 66 + i * 20, 6, 1, CHECKER_SIZE, 0xFFFFFF);
        plain_rect(pattern_scr, 6, 86 + i * 8, 60, 1, 0xFFFFFF);
    }

    /* Pure color bars, full width. In the landscape variant the
     * vertical rhythm (bars and text lines) is compacted so that
     * everything still fits, in 240 rows instead of 280. */
    static const uint32_t bar_colors[] = {0xFF0000, 0x00FF00, 0x0000FF,
                                          0xFFFFFF, 0x808080, 0x000000};
#if defined(CONFIG_ESB_NAPE_TOUCHTEST_LANDSCAPE_PATTERN)
    const int32_t bar_y0 = 110;
    const int32_t bar_step = 8;
    const int32_t bar_h = 7;
#else
    const int32_t bar_y0 = 112;
    const int32_t bar_step = 10;
    const int32_t bar_h = 9;
#endif

    for (int i = 0; i < 6; i++) {
        plain_rect(pattern_scr, 6, bar_y0 + i * bar_step, W - 12, bar_h, bar_colors[i]);
    }

    /* Text samples in every compiled font size (12/14/20/28 — this
     * tree has no montserrat 16), white on black. */
    static const struct {
        const lv_font_t *font;
        const char *text;
    } samples[] = {
        {&lv_font_montserrat_12, "SHARP 12"},
        {&lv_font_montserrat_14, "SHARP 14"},
        {&lv_font_montserrat_20, "SHARP 20"},
        {&lv_font_montserrat_28, "SHARP 28"},
    };
#if defined(CONFIG_ESB_NAPE_TOUCHTEST_LANDSCAPE_PATTERN)
    static const int32_t advances[] = {13, 15, 21, 29};
    int32_t ty = 160;
#else
    static const int32_t advances[] = {15, 19, 25, 31};
    int32_t ty = 176;
#endif

    for (int i = 0; i < 4; i++) {
        lv_obj_t *label = lv_label_create(pattern_scr);

        lv_obj_set_style_text_font(label, samples[i].font, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_label_set_text(label, samples[i].text);
        lv_obj_set_pos(label, 6, ty);
        ty += advances[i];
    }

    /* BACK, bottom-right (the SHARP 28 label ends before it). */
    lv_obj_t *back_btn = styled_button(pattern_scr, 0x2266CC, back_btn_event_cb);

    lv_obj_set_size(back_btn, 76, 34);
    lv_obj_set_pos(back_btn, W - 82, H - 40);
    button_label(back_btn, "BACK", &lv_font_montserrat_14);

    /* NEXT to pattern page 2 (v7), top-center: the only free spot
     * on this page in both geometries — the bottom rows carry the
     * SHARP samples on the left and BACK on the right, the top
     * corners carry the checkerboard and the 1 px lines. */
    lv_obj_t *next_btn = styled_button(pattern_scr, 0x338833, next_btn_event_cb);

    lv_obj_set_size(next_btn, 76, 34);
    lv_obj_align(next_btn, LV_ALIGN_TOP_MID, 0, 6);
    button_label(next_btn, "NEXT", &lv_font_montserrat_14);
}

/* ---- Pattern page 2 (v7): text-rendering discriminators ---- */

static lv_obj_t *p2_label(lv_obj_t *parent, const lv_font_t *font, const char *text, int32_t x,
                          int32_t y, uint32_t color_hex) {
    lv_obj_t *label = lv_label_create(parent);

    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(color_hex), LV_PART_MAIN);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    return label;
}

static void build_pattern2_screen(void) {
    const int32_t W = lv_display_get_horizontal_resolution(NULL);
    const int32_t H = lv_display_get_vertical_resolution(NULL);

    pattern2_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(pattern2_scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(pattern2_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(pattern2_scr, LV_OBJ_FLAG_SCROLLABLE);

    /* Sections stack on a running y cursor (v8) so the optional
     * custom-font samples can slot in without pushing the last
     * section past the 240x240 common area; the section metrics
     * are a few px tighter than v7's absolute layout. */
    int32_t y = 4;

    /* 1. Bitmap-font text: unscii glyphs are 1 bpp — no
     * antialiasing anywhere in their path. Advances are fixed
     * (8 px/char at unscii_8, 16 px/char at unscii_16), and the
     * strings are sized to the 228 px usable width. */
    p2_label(pattern2_scr, &lv_font_unscii_8, "UNSCII ABCDEFGHIJ 0123456789", 6, y, 0xFFFFFF);
    y += 8;
    p2_label(pattern2_scr, &lv_font_unscii_8, "THE QUICK BROWN FOX JUMPS", 6, y, 0xFFFFFF);
    y += 9;
#if LV_FONT_UNSCII_16
    p2_label(pattern2_scr, &lv_font_unscii_16, "UNSCII16 ABCDE", 6, y, 0xFFFFFF);
    y += 17;
#endif

    /* 2. AA control text: the same words in the stock Montserrat
     * (bpp 4). The strings are trimmed to fit 240 px at these
     * sizes (the full "... 0123456789" tail only fits at 14). */
    p2_label(pattern2_scr, &lv_font_montserrat_14, "MONTSERRAT ABCDEFG 0123", 6, y, 0xFFFFFF);
    y += 15;
    p2_label(pattern2_scr, &lv_font_montserrat_20, "MONTSERRAT ABC", 6, y, 0xFFFFFF);
    y += 19;

#if defined(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
    /* 2b. Custom-font samples (v8): the SemiBold the Deck UI
     * uses, and a Bold for comparison, at the same 20 px as the
     * stock control above. The SemiBold line drops the digit
     * tail: at SemiBold advances the full string measures
     * ~270 px and would run off the 240 px stage (the Bold line
     * at ~220 px fits whole). */
    p2_label(pattern2_scr, &lv_font_montserrat_sb_20, "SEMIBOLD ABCDEFG", 6, y, 0xFFFFFF);
    y += 19;
    p2_label(pattern2_scr, &lv_font_montserrat_b_20, "BOLD ABCDEFG 0123", 6, y, 0xFFFFFF);
    y += 20;

    /* 2c. Small-size SemiBold samples: does the heavier face
     * make the sizes below the UI's 16 px floor usable again?
     * (Expectation: 14 is the floor.) Judging samples only —
     * the production UI stays at 16/20/28 until this is judged
     * on hardware. Both strings fit whole at these sizes
     * (~204 px at 14, ~173 px at 12, measured from the TTF). */
    p2_label(pattern2_scr, &lv_font_montserrat_sb_14, "SEMIBOLD14 ABCDEFG 0123", 6, y, 0xFFFFFF);
    y += 16;
    p2_label(pattern2_scr, &lv_font_montserrat_sb_12, "SEMIBOLD12 ABCDEFG 0123", 6, y, 0xFFFFFF);
    y += 15;
#endif

    /* 3. Solid-gray stroke mimics: groups of 2 px / 3 px vertical
     * bars, glyph-stroke height, filled with plain grays. AA edge
     * pixels are exactly such mid-grays produced by blending; if
     * these solid bars are crisp while Montserrat strokes smear,
     * mid-tone reproduction is fine and the blend/glyph data is
     * the problem. Captions are unscii so they carry no AA of
     * their own. */
    static const uint32_t grays[] = {0x404040, 0x808080, 0xBFBFBF, 0xFFFFFF};
    static const char *const gray_names[] = {"25", "50", "75", "100"};
    static const int32_t cap_dx[] = {4, 4, 4, 0}; /* center the caption over the 24 px group */
    static const int32_t bar_dx[] = {0, 5, 11, 16, 22};
    static const int32_t bar_w[] = {2, 3, 2, 3, 2};

    for (int g = 0; g < 4; g++) {
        const int32_t gx = 6 + g * 57;

        for (int b = 0; b < 5; b++) {
            plain_rect(pattern2_scr, gx + bar_dx[b], y, bar_w[b], 12, grays[g]);
        }
        p2_label(pattern2_scr, &lv_font_unscii_8, gray_names[g], gx + cap_dx[g], y + 13, 0xFFFFFF);
    }
    y += 23;

    /* 4. White Montserrat 20 blended over a solid mid-gray fill
     * instead of black: exercises the blend against a non-black
     * background. */
    plain_rect(pattern2_scr, 6, y, 228, 24, 0x808080);
    p2_label(pattern2_scr, &lv_font_montserrat_20, "TEXT ON GRAY", 12, y + 2, 0xFFFFFF);
    y += 25;

    /* 5. 16-step gray ramp: adjacent solid bars, black to white
     * in steps of 17 — mid-tone solids at fine granularity. */
    for (int i = 0; i < 16; i++) {
        const uint32_t v = (uint32_t)i * 17u;

        plain_rect(pattern2_scr, 6 + i * 14, y, 14, 10, (v << 16) | (v << 8) | v);
    }

    /* BACK to pattern page 1, bottom-right like page 1's BACK. */
    lv_obj_t *back_btn = styled_button(pattern2_scr, 0x2266CC, back2_btn_event_cb);

    lv_obj_set_size(back_btn, 76, 34);
    lv_obj_set_pos(back_btn, W - 82, H - 40);
    button_label(back_btn, "BACK", &lv_font_montserrat_14);
}

static void screen_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    build_tap_screen();
    build_pattern_screen();
    build_pattern2_screen();

#if defined(CONFIG_ESB_NAPE_TOUCHTEST_LANDSCAPE_PATTERN)
    /* Landscape bisect variant: show the pattern immediately.
     * Touch coordinates do not map onto the rotated geometry, so
     * tap navigation is meaningless in this build anyway. */
    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
#else
    lv_scr_load(tap_scr);
    LOG_INF("TOUCHTEST tap screen loaded");
#endif
}

static K_WORK_DEFINE(screen_work, screen_work_handler);

static void boot_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    /* Display init runs on the display queue during ZMK startup; by
     * the time this fires (seconds after boot) the status screen is
     * up, so loading the tap screen over it from that same queue is
     * race-free. */
    k_work_submit_to_queue(zmk_display_work_q(), &screen_work);
}

static K_WORK_DELAYABLE_DEFINE(boot_work, boot_work_handler);

static int touchtest_init(void) {
    k_work_schedule(&boot_work, K_SECONDS(2));
    k_work_schedule(&scan_work, K_SECONDS(4));
    return 0;
}

SYS_INIT(touchtest_init, APPLICATION, 99);
