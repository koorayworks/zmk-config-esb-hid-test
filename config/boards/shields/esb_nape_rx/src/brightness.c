/*
 * Display backlight control for Nape: PWM LED on P0.16 (hidden pad).
 * Follows ZMK activity state: active 85%, idle 30%, sleep 0%.
 */

#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>

#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>

#define BL_NODE DT_NODELABEL(disp_bl)

static const struct device *const pwm_leds_dev = DEVICE_DT_GET_ONE(pwm_leds);
#define DISP_BL_IDX DT_NODE_CHILD_IDX(BL_NODE)

static int bl_set(uint8_t pct) {
    if (!device_is_ready(pwm_leds_dev)) {
        return -ENODEV;
    }
    return led_set_brightness(pwm_leds_dev, DISP_BL_IDX, pct);
}

static int on_activity_state(const zmk_event_t *eh) {
    const struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    switch (ev->state) {
    case ZMK_ACTIVITY_ACTIVE:
        bl_set(85);
        break;
    case ZMK_ACTIVITY_IDLE:
        bl_set(30);
        break;
    case ZMK_ACTIVITY_SLEEP:
        bl_set(0);
        break;
    }
    return 0;
}

ZMK_LISTENER(nape_backlight, on_activity_state);
ZMK_SUBSCRIPTION(nape_backlight, zmk_activity_state_changed);

static int nape_bl_init(void) {
    bl_set(85);
    return 0;
}

SYS_INIT(nape_bl_init, APPLICATION, 90);
