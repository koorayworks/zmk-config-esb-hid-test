/*
 * Touch bring-up test for the Nape Deck (esb_nape_touchtest only).
 *
 *  - ~3 s after boot, scans the i2c0 bus and logs every address that
 *    ACKs, to find where the CST816S (expected at 0x15) actually
 *    answers with the Scanner harness wiring.
 *  - Loads a screen with a single big button over the status screen;
 *    every press increments the on-screen counter and logs
 *    "TOUCHTEST tap #N", proving the chain CST816S -> input
 *    subsystem -> LVGL pointer input end to end.
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

static const struct device *const i2c_bus = DEVICE_DT_GET(DT_NODELABEL(i2c0));

/* ---- I2C bus scan (system work queue, ~3 s after boot) ---- */

static void scan_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!device_is_ready(i2c_bus)) {
        LOG_ERR("TOUCHTEST scan: i2c0 not ready");
        return;
    }

    int found = 0;

    for (uint16_t addr = 0x08; addr <= 0x77; addr++) {
        uint8_t byte;

        /* A 1-byte read is the portable probe: devices that ACK
         * their address return success, everything else errors. */
        if (i2c_read(i2c_bus, &byte, 1, addr) == 0) {
            LOG_INF("TOUCHTEST scan: ACK at 0x%02x", addr);
            found++;
        }
    }

    LOG_INF("TOUCHTEST scan done: %d devices", found);
}

static K_WORK_DELAYABLE_DEFINE(scan_work, scan_work_handler);

/* ---- Tap test screen (created/loaded on the display work queue) ---- */

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

static void screen_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    lv_obj_t *scr = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t *btn = lv_button_create(scr);

    lv_obj_set_size(btn, 220, 160);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, tap_btn_event_cb, LV_EVENT_PRESSED, NULL);

    tap_label = lv_label_create(btn);
    lv_obj_set_style_text_font(tap_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text(tap_label, "TAP TEST: 0");
    lv_obj_center(tap_label);

    lv_scr_load(scr);
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
    k_work_schedule(&scan_work, K_SECONDS(3));
    return 0;
}

SYS_INIT(touchtest_init, APPLICATION, 99);
