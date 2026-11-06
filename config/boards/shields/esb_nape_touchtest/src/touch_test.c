/*
 * Touch bring-up test for the Nape Deck (esb_nape_touchtest only) — v11.
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
 * v9: the bpp 4 SemiBold smeared on hardware exactly like the
 * stock font (and the color fringes on glyph edges are visible
 * to the naked eye), so section 2b of page 2 becomes the bpp
 * axis of the SemiBold face: bpp 8 (via the
 * ESB_NAPE_RX_FONT_BPP8 switch — also the status UI default of
 * this build), bpp 4 and bpp 1 at 20 px with unscii captions,
 * plus SemiBold 14 at bpp 1. The v8 Bold/small-size AA samples
 * retired and the other sections tightened to keep the page
 * inside the common area.
 *
 * v10: verdict in (2026-11-06) — the bpp 1 samples render clean
 * on hardware, bpp 4/8 smear, so the status UI switched to the
 * bpp 1 set via ESB_NAPE_RX_FONT_BPP1 (top of its ladder). Page
 * 2 itself is unchanged: the BPP8 switch stays on in this
 * shield's conf purely so the first sample line keeps showing
 * the bpp 8 conversion for comparison.
 *
 * v11 adds LIVE screen rotation (user requirement 2026-11-07:
 * rotation must switch live, like the Nape's trackball
 * orientation — no reboot-to-apply). A ROTATE button on the tap
 * screen cycles 0 -> 90 -> 180 -> 270 -> 0 at runtime. Route
 * (source-verified in ~/workspace/esb-hid/rotation-design.md's
 * follow-up): LVGL's own software rotation cannot work here —
 * lv_refr.c never rotates pixels; rotation is the flush
 * driver's job (fb drivers call lv_display_rotate_area() +
 * lv_draw_sw_rotate()), and the Zephyr glue's flush callback is
 * a raw passthrough. Instead the panel itself rotates: the
 * forked ST7789V driver in zmk-feature-esb-hid implements
 * display_set_orientation() (MADCTL + margins), and the app
 * then re-frames LVGL around the rotated panel with
 * lv_display_set_resolution(), which resizes every screen and
 * forces a full re-render in the new logical frame (PARTIAL
 * render mode reshapes the draw buffer per area, so the
 * init-time buffer geometry is not a constraint). Touch follows
 * because the Zephyr pointer glue transforms every event
 * against the capabilities cached in the display's user data
 * (struct lvgl_disp_data, <lvgl_display.h>) — the rotation work
 * updates that cache (orientation + x/y resolution) in the same
 * step. All of it runs as a work item on the display queue, so
 * it cannot interleave with an LVGL refresh or with the button
 * event that triggered it. The three screens are rebuilt at the
 * new resolution (their layouts already read the live
 * resolution) and the current one is reloaded. In the
 * landscape-pattern variant the ROTATE button is compiled out:
 * that build's boot geometry comes from DT, not from this
 * runtime state, so cycling would start from a lie.
 *
 * The production status screen is not modified: esb_nape_rx still
 * creates it, and this screen is simply loaded over it once the
 * display is up (its refresh timer keeps updating the covered
 * screen, which is harmless for a bring-up build).
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <lvgl_display.h>
#include <zmk/display.h>

LOG_MODULE_REGISTER(touchtest);

/* Custom-font comparison samples for pattern page 2 (v8). The
 * font objects are compiled by the esb_nape_rx shield library
 * (its src/fonts/), so they are only declared here — this file
 * must not reference them when the config is off. Page 2 lines
 * up the bpp axis of the same SemiBold face: bpp 8 (via the
 * ESB_NAPE_RX_FONT_BPP8 switch), bpp 4 and bpp 1 at 20 px, plus
 * 14 px at bpp 1. (The status UI's own default is bpp 1 since
 * v10 — this page keeps all three for comparison.) */
#if defined(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
LV_FONT_DECLARE(lv_font_montserrat_sb_20);
LV_FONT_DECLARE(lv_font_montserrat_sb1_14);
LV_FONT_DECLARE(lv_font_montserrat_sb1_20);
#if defined(CONFIG_ESB_NAPE_RX_FONT_BPP8)
LV_FONT_DECLARE(lv_font_montserrat_sb8_20);
#define P2_SB_FONT_20 (&lv_font_montserrat_sb8_20)
#define P2_SB20_CAPTION "BPP8"
#else
#define P2_SB_FONT_20 (&lv_font_montserrat_sb_20)
#define P2_SB20_CAPTION "BPP4"
#endif
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

/* Which of the three screens is on display (0 tap, 1 pattern,
 * 2 pattern 2); the rotation work reloads the same page after
 * rebuilding all screens at the new resolution. */
static int cur_scr;

/* Live rotation state (v11). rot_idx indexes the cycle; the
 * logical resolution follows from it (odd steps swap the axes). */
static const struct device *const panel = DEVICE_DT_GET(DT_NODELABEL(st7789v));
static const enum display_orientation rot_cycle[4] = {
    DISPLAY_ORIENTATION_NORMAL,
    DISPLAY_ORIENTATION_ROTATED_90,
    DISPLAY_ORIENTATION_ROTATED_180,
    DISPLAY_ORIENTATION_ROTATED_270,
};
static uint8_t rot_idx;

static void rotate_work_handler(struct k_work *work);
static K_WORK_DEFINE(rotate_work, rotate_work_handler);

static void rotate_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    /* Only queue the switch: the screens cannot be rebuilt from
     * inside this event (the button itself is on one of them),
     * and the display queue serializes the work against LVGL
     * refreshes. */
    k_work_submit_to_queue(zmk_display_work_q(), &rotate_work);
    LOG_INF("TOUCHTEST rotate queued");
}

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

    cur_scr = 1;
    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
}

static void back_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 0;
    lv_scr_load(tap_scr);
    LOG_INF("TOUCHTEST tap screen loaded");
}

static void next_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 2;
    lv_scr_load(pattern2_scr);
    LOG_INF("TOUCHTEST pattern 2 screen loaded");
}

static void back2_btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 1;
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

    /* In the 240-row orientations the column compresses so the
     * ROTATE row fits below PATTERN; at 280 rows the layout is
     * the historical one plus the new row. */
    const int32_t scr_h = lv_display_get_vertical_resolution(NULL);
    const bool compact = (scr_h <= 240);

    lv_obj_t *pattern_btn = styled_button(tap_scr, 0x338833, pattern_btn_event_cb);

    lv_obj_set_size(pattern_btn, 220, compact ? 40 : 56);
    lv_obj_align(pattern_btn, LV_ALIGN_TOP_MID, 0, compact ? 152 : 168);
    button_label(pattern_btn, "PATTERN", &lv_font_montserrat_20);

#if !defined(CONFIG_ESB_NAPE_TOUCHTEST_LANDSCAPE_PATTERN)
    /* Live rotation (v11). Compiled out of the landscape
     * variant: its boot geometry is DT-driven, so this runtime
     * cycle state would start out of sync with the panel. */
    lv_obj_t *rotate_btn = styled_button(tap_scr, 0xCC7722, rotate_btn_event_cb);

    lv_obj_set_size(rotate_btn, 220, compact ? 36 : 40);
    lv_obj_align(rotate_btn, LV_ALIGN_TOP_MID, 0, compact ? 200 : 232);
    button_label(rotate_btn, "ROTATE", &lv_font_montserrat_20);
#endif
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
    y += 16;
#endif

    /* 2. AA control text: the same words in the stock Montserrat
     * (bpp 4). The strings are trimmed to fit 240 px at these
     * sizes (the full "... 0123456789" tail only fits at 14). */
    p2_label(pattern2_scr, &lv_font_montserrat_14, "MONTSERRAT ABCDEFG 0123", 6, y, 0xFFFFFF);
    y += 14;
    p2_label(pattern2_scr, &lv_font_montserrat_20, "MONTSERRAT ABC", 6, y, 0xFFFFFF);
    y += 18;

#if defined(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
    /* 2b. The bpp axis in one glance (v9): the same SemiBold
     * 20 sample ("ABCDEFG 0123", 152 px from the TTF) at
     * bpp 8 (via the switch — the UI default of this build),
     * bpp 4 and bpp 1, plus SemiBold 14 at bpp 1 (107 px,
     * started further right to clear its longer caption).
     * Captions are unscii_8 so they carry no AA of their own.
     * Why both directions: partial-coverage pixels show color
     * fringes to the naked eye on this panel (2026-11-06), so
     * MORE alpha steps may look worse and bpp 1 — no
     * intermediate pixels at all — is a fix candidate, not
     * just a control. (With the BPP8 switch off, the first
     * line falls back to bpp 4 and duplicates the second.)
     * The v8 Bold 20 and small-size AA samples retired to make
     * room; sections 1/3/4/5 are tightened by a few px each. */
    p2_label(pattern2_scr, &lv_font_unscii_8, P2_SB20_CAPTION, 6, y + 6, 0xFFFFFF);
    p2_label(pattern2_scr, P2_SB_FONT_20, "ABCDEFG 0123", 48, y, 0xFFFFFF);
    y += 21;
    p2_label(pattern2_scr, &lv_font_unscii_8, "BPP4", 6, y + 6, 0xFFFFFF);
    p2_label(pattern2_scr, &lv_font_montserrat_sb_20, "ABCDEFG 0123", 48, y, 0xFFFFFF);
    y += 21;
    p2_label(pattern2_scr, &lv_font_unscii_8, "BPP1", 6, y + 6, 0xFFFFFF);
    p2_label(pattern2_scr, &lv_font_montserrat_sb1_20, "ABCDEFG 0123", 48, y, 0xFFFFFF);
    y += 21;
    p2_label(pattern2_scr, &lv_font_unscii_8, "BPP1-14", 6, y + 3, 0xFFFFFF);
    p2_label(pattern2_scr, &lv_font_montserrat_sb1_14, "ABCDEFG 0123", 70, y, 0xFFFFFF);
    y += 17;
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
            plain_rect(pattern2_scr, gx + bar_dx[b], y, bar_w[b], 10, grays[g]);
        }
        p2_label(pattern2_scr, &lv_font_unscii_8, gray_names[g], gx + cap_dx[g], y + 11, 0xFFFFFF);
    }
    y += 20;

    /* 4. White Montserrat 20 blended over a solid mid-gray fill
     * instead of black: exercises the blend against a non-black
     * background. */
    plain_rect(pattern2_scr, 6, y, 228, 22, 0x808080);
    p2_label(pattern2_scr, &lv_font_montserrat_20, "TEXT ON GRAY", 12, y + 1, 0xFFFFFF);
    y += 23;

    /* 5. 16-step gray ramp: adjacent solid bars, black to white
     * in steps of 17 — mid-tone solids at fine granularity. */
    for (int i = 0; i < 16; i++) {
        const uint32_t v = (uint32_t)i * 17u;

        plain_rect(pattern2_scr, 6 + i * 14, y, 14, 8, (v << 16) | (v << 8) | v);
    }

    /* BACK to pattern page 1, bottom-right like page 1's BACK. */
    lv_obj_t *back_btn = styled_button(pattern2_scr, 0x2266CC, back2_btn_event_cb);

    lv_obj_set_size(back_btn, 76, 34);
    lv_obj_set_pos(back_btn, W - 82, H - 40);
    button_label(back_btn, "BACK", &lv_font_montserrat_14);
}

/* Live rotation switch (v11). Runs on the display queue, never
 * concurrently with an LVGL refresh or a touch event. Order:
 * panel first (MADCTL + margins via the forked driver), then the
 * pointer glue's cached capabilities (it transforms every event
 * against that cache, read live), then LVGL's logical frame
 * (resizes all screens and forces a full re-render), then a
 * rebuild of the three test screens at the new resolution. */
static void rotate_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    rot_idx = (rot_idx + 1) & 0x3;
    const enum display_orientation zo = rot_cycle[rot_idx];
    const int32_t w = (rot_idx & 1) ? 280 : 240;
    const int32_t h = (rot_idx & 1) ? 240 : 280;

    if (!device_is_ready(panel)) {
        LOG_ERR("TOUCHTEST rotate: panel not ready");
        return;
    }

    int ret = display_set_orientation(panel, zo);

    if (ret != 0) {
        LOG_ERR("TOUCHTEST rotate: set_orientation -> %d", ret);
        return;
    }

    lv_display_t *disp = lv_display_get_default();
    struct lvgl_disp_data *dd = lv_display_get_user_data(disp);

    if (dd != NULL) {
        dd->cap.current_orientation = zo;
        dd->cap.x_resolution = w;
        dd->cap.y_resolution = h;
    }
    lv_display_set_resolution(disp, w, h);

    lv_obj_t *old_tap = tap_scr;
    lv_obj_t *old_pattern = pattern_scr;
    lv_obj_t *old_pattern2 = pattern2_scr;

    build_tap_screen();
    build_pattern_screen();
    build_pattern2_screen();
    lv_scr_load(cur_scr == 2 ? pattern2_scr : (cur_scr == 1 ? pattern_scr : tap_scr));
    if (old_tap != NULL) {
        lv_obj_delete(old_tap);
    }
    if (old_pattern != NULL) {
        lv_obj_delete(old_pattern);
    }
    if (old_pattern2 != NULL) {
        lv_obj_delete(old_pattern2);
    }

    LOG_INF("TOUCHTEST rotate -> %d deg (%dx%d)", rot_idx * 90, (int)w, (int)h);
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
    cur_scr = 1;
    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
#else
    cur_scr = 0;
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
