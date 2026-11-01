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

    /* Pure color bars, full width. */
    static const uint32_t bar_colors[] = {0xFF0000, 0x00FF00, 0x0000FF,
                                          0xFFFFFF, 0x808080, 0x000000};
    const int32_t bar_y0 = 112;

    for (int i = 0; i < 6; i++) {
        plain_rect(pattern_scr, 6, bar_y0 + i * 10, W - 12, 9, bar_colors[i]);
    }

    /* Text samples in every compiled font size (12/14/20/28 — this
     * tree has no montserrat 16), white on black. */
    static const struct {
        const lv_font_t *font;
        const char *text;
        int32_t advance;
    } samples[] = {
        {&lv_font_montserrat_12, "SHARP 12", 15},
        {&lv_font_montserrat_14, "SHARP 14", 19},
        {&lv_font_montserrat_20, "SHARP 20", 25},
        {&lv_font_montserrat_28, "SHARP 28", 31},
    };
    int32_t ty = 176;

    for (int i = 0; i < 4; i++) {
        lv_obj_t *label = lv_label_create(pattern_scr);

        lv_obj_set_style_text_font(label, samples[i].font, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_label_set_text(label, samples[i].text);
        lv_obj_set_pos(label, 6, ty);
        ty += samples[i].advance;
    }

    /* BACK, bottom-right (the SHARP 28 label ends before it). */
    lv_obj_t *back_btn = styled_button(pattern_scr, 0x2266CC, back_btn_event_cb);

    lv_obj_set_size(back_btn, 76, 34);
    lv_obj_set_pos(back_btn, W - 82, H - 40);
    button_label(back_btn, "BACK", &lv_font_montserrat_14);
}

static void screen_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    build_tap_screen();
    build_pattern_screen();

    lv_scr_load(tap_scr);
    LOG_INF("TOUCHTEST tap screen loaded");
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
