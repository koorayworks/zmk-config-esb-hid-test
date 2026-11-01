/*
 * Touch bring-up test for the Nape Deck (esb_nape_touchtest only) — v2.
 *
 *  - ~3 s after boot, bit-bangs an I2C address scan over the
 *    candidate (SDA, SCL) pin pairs on raw GPIOs and logs every
 *    address that ACKs, to find where the CST816S (expected at
 *    0x15) is actually wired on the Scanner harness:
 *        (P1.00, P1.10)  production hidden-pad plan (HW I2C: silent)
 *        (P1.10, P1.00)  same pair, swapped
 *        (P1.02, P1.04)  XIAO standard I2C D4/D5 (v1 HW I2C scan:
 *                        silent — but in v1 the parked dummy SPI may
 *                        have held the chip in reset; the parking is
 *                        gone in this build, so retest cleanly)
 *        (P1.04, P1.02)  D5/D4, swapped
 *    Before scanning, D0/D1/D2 (P0.02/P0.03/P0.28) are configured as
 *    inputs with pull-ups: if the harness lands the touch RST (or
 *    any other touch line) there, this releases it instead of
 *    leaving it driven by the parked SPI pinctrl of the disptest
 *    layer (which this variant disables outright — see the overlay).
 *  - Loads a screen with a single explicitly styled button over the
 *    status screen; every press increments the on-screen counter
 *    and logs "TOUCHTEST tap #N", proving the chain CST816S ->
 *    input subsystem -> LVGL pointer input end to end. The v1
 *    button relied on the default theme and rendered invisible;
 *    v2 sets its colors explicitly.
 *
 * The production status screen is not modified: esb_nape_rx still
 * creates it, and this screen is simply loaded over it once the
 * display is up (its refresh timer keeps updating the covered
 * screen, which is harmless for a bring-up build).
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <zmk/display.h>

LOG_MODULE_REGISTER(touchtest);

static const struct device *const gpio0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const struct device *const gpio1_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));

/* ---- Bit-bang I2C scan (system work queue, ~3 s after boot) ----
 *
 * Open-drain emulation with raw GPIO: drive a line low with
 * GPIO_OUTPUT_LOW, release it by switching back to a pulled-up
 * input, and read SDA while it is released. Conservative timings
 * (~25 us per phase, ~40 kHz) so the probe works even with weak
 * pull-ups on the harness.
 */

#define BB_HALF_US 25

struct bb_pair {
    uint8_t sda_pin; /* on gpio1 */
    uint8_t scl_pin; /* on gpio1 */
    const char *sda_name;
    const char *scl_name;
};

static void bb_drive_low(uint8_t pin) { gpio_pin_configure(gpio1_dev, pin, GPIO_OUTPUT_LOW); }

static void bb_release(uint8_t pin) { gpio_pin_configure(gpio1_dev, pin, GPIO_INPUT | GPIO_PULL_UP); }

static int bb_read_sda(uint8_t pin) { return gpio_pin_get(gpio1_dev, pin); }

static void bb_start(const struct bb_pair *p) {
    bb_release(p->sda_pin);
    bb_release(p->scl_pin);
    k_busy_wait(BB_HALF_US);
    bb_drive_low(p->sda_pin);
    k_busy_wait(BB_HALF_US);
    bb_drive_low(p->scl_pin);
    k_busy_wait(BB_HALF_US);
}

static void bb_stop(const struct bb_pair *p) {
    bb_drive_low(p->sda_pin);
    k_busy_wait(BB_HALF_US);
    bb_release(p->scl_pin);
    k_busy_wait(BB_HALF_US);
    bb_release(p->sda_pin);
    k_busy_wait(BB_HALF_US);
}

/* Shift out one byte (write direction) and sample the ACK bit. */
static bool bb_probe(const struct bb_pair *p, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        if (byte & 0x80) {
            bb_release(p->sda_pin);
        } else {
            bb_drive_low(p->sda_pin);
        }
        k_busy_wait(BB_HALF_US);
        bb_release(p->scl_pin);
        k_busy_wait(BB_HALF_US);
        bb_drive_low(p->scl_pin);
        k_busy_wait(BB_HALF_US);
        byte <<= 1;
    }

    bb_release(p->sda_pin);
    k_busy_wait(BB_HALF_US);
    bb_release(p->scl_pin);
    k_busy_wait(BB_HALF_US);
    bool ack = bb_read_sda(p->sda_pin) == 0;
    bb_drive_low(p->scl_pin);
    k_busy_wait(BB_HALF_US);

    return ack;
}

static const struct bb_pair bb_pairs[] = {
    {0, 10, "P1.00", "P1.10"},
    {10, 0, "P1.10", "P1.00"},
    {2, 4, "P1.02", "P1.04"},
    {4, 2, "P1.04", "P1.02"},
};

static void scan_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!device_is_ready(gpio0_dev) || !device_is_ready(gpio1_dev)) {
        LOG_ERR("TOUCHTEST bbscan: gpio not ready");
        return;
    }

    /* Release D0/D1/D2 first: if the harness lands the touch RST
     * (or another touch line) on one of them, a pulled-up input
     * lets the chip run instead of holding it in reset. */
    static const uint8_t harness_pins[] = {2, 3, 28}; /* P0.02, P0.03, P0.28 */

    for (size_t i = 0; i < ARRAY_SIZE(harness_pins); i++) {
        gpio_pin_configure(gpio0_dev, harness_pins[i], GPIO_INPUT | GPIO_PULL_UP);
    }
    /* Give a chip that was held in reset a moment to boot before
     * its address is probed. */
    k_msleep(50);

    for (size_t i = 0; i < ARRAY_SIZE(bb_pairs); i++) {
        const struct bb_pair *p = &bb_pairs[i];

        bb_release(p->sda_pin);
        bb_release(p->scl_pin);
        k_busy_wait(4 * BB_HALF_US);

        for (uint16_t addr = 0x08; addr <= 0x77; addr++) {
            bb_start(p);
            bool ack = bb_probe(p, (uint8_t)(addr << 1));
            bb_stop(p);

            if (ack) {
                LOG_INF("TOUCHTEST bbscan SDA=%s SCL=%s -> ACK 0x%02x", p->sda_name, p->scl_name,
                        addr);
            }
        }

        /* Leave the pair released for the next candidate. */
        bb_release(p->sda_pin);
        bb_release(p->scl_pin);
    }

    LOG_INF("TOUCHTEST bbscan done");
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
    /* Explicit styles: this build's default theme renders a plain
     * button (nearly) invisible, which made v1's screen look dead. */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2266CC), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 12, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, tap_btn_event_cb, LV_EVENT_PRESSED, NULL);

    tap_label = lv_label_create(btn);
    lv_obj_set_style_text_font(tap_label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(tap_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
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
