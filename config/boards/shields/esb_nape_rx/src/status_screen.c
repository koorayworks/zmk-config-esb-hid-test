/*
 * Nape status screen (v1 skeleton).
 *
 * Design: pure black background, no decorations, hairline separators.
 * Shows the ESB link state of device 0 from the ESB HID status store.
 * Full layout (per-layer colors, switcher, link stats) comes later;
 * this version proves the display + status plumbing.
 */

#include <lvgl.h>
#include <zephyr/kernel.h>

#include <esb_hid_status.h>
#include <zmk/display/status_screen.h>

static lv_obj_t *kb_name_label;
static lv_obj_t *layer_label;
static lv_obj_t *batt_label;

static void refresh_cb(lv_timer_t *timer) {
    struct esb_hid_device_status st;

    if (!esb_hid_get_device_status(0, &st) || !st.valid) {
        lv_label_set_text(kb_name_label, "NO LINK");
        lv_label_set_text(layer_label, "");
        lv_label_set_text(batt_label, "");
        return;
    }

    lv_label_set_text(kb_name_label, "KEYBOARD");
    lv_label_set_text_fmt(layer_label, "LAYER %d", st.layer);
    if (st.battery == 0xFF) {
        lv_label_set_text(batt_label, "--%");
    } else {
        lv_label_set_text_fmt(batt_label, "%d%%", st.battery);
    }
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    /* Pure black background, nothing else drawn. */
    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_color(screen, lv_color_white(), LV_PART_MAIN);

    lv_obj_t *brand = lv_label_create(screen);
    lv_label_set_text(brand, "NAPE");
    lv_obj_set_style_text_color(brand, lv_color_make(0x5C, 0x5C, 0x60), LV_PART_MAIN);
    lv_obj_align(brand, LV_ALIGN_TOP_LEFT, 16, 12);

    kb_name_label = lv_label_create(screen);
    lv_label_set_text(kb_name_label, "NO LINK");
    lv_obj_align(kb_name_label, LV_ALIGN_TOP_LEFT, 16, 48);

    layer_label = lv_label_create(screen);
    lv_obj_align(layer_label, LV_ALIGN_TOP_LEFT, 16, 76);

    batt_label = lv_label_create(screen);
    lv_obj_align(batt_label, LV_ALIGN_TOP_LEFT, 16, 104);

    lv_timer_create(refresh_cb, 1000, NULL);

    return screen;
}
