/*
 * Nape status screen — home screen per the approved UI design (v2):
 * pure black background, 240x240 square stage centered in the panel,
 * white/gray text only, amber reserved for "active" markers, per-layer
 * colors for the layer name and dots.
 *
 * Data comes from the ESB HID status store (device 0). The keyboard
 * name is a placeholder until the protocol carries names (Step 2).
 */

#include <lvgl.h>
#include <zephyr/kernel.h>

#include <esb_hid_status.h>
#include <zmk/battery.h>
#include <zmk/display/status_screen.h>

#define COL_SUB 0x9A9AA0
#define COL_MUTE 0x5C5C60
#define COL_LINE 0x26262A
#define COL_DOT_OFF 0x3A3A3E
#define COL_AMBER 0xFFB340
#define COL_ALERT 0xFF453A

/* Per-layer colors (TentAlice RGB indicator assignment). */
static const uint32_t layer_colors[] = {
    0xFFFFFF, /* L0 base: white */
    0xFF453A, /* L1 red */
    0x32D74B, /* L2 green */
    0xFFD60A, /* L3 yellow */
    0x0A84FF, /* L4 blue */
    0xBF5AF2, /* L5 violet */
};
#define LAYER_COUNT (sizeof(layer_colors) / sizeof(layer_colors[0]))

static lv_obj_t *link_dot;
static lv_obj_t *own_batt_label;
static lv_obj_t *kb_name_label;
static lv_obj_t *layer_label;
static lv_obj_t *layer_dots[LAYER_COUNT];
static lv_obj_t *batt_pct_label;
static lv_obj_t *batt_gauge_fg;
static lv_obj_t *row_radio;
static lv_obj_t *row_name_label;
static lv_obj_t *row_pct_label;

static lv_obj_t *make_label(lv_obj_t *parent, const lv_font_t *font, uint32_t color, int x, int y) {
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_pos(label, x, y);
    return label;
}

static lv_obj_t *make_dot(lv_obj_t *parent, int x, int y, int size, uint32_t color) {
    lv_obj_t *dot = lv_obj_create(parent);
    lv_obj_set_size(dot, size, size);
    lv_obj_set_pos(dot, x, y);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dot, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
    return dot;
}

static void refresh_cb(lv_timer_t *timer) {
    struct esb_hid_device_status st;
    bool linked = esb_hid_get_device_status(0, &st) && st.valid;

    lv_obj_set_style_bg_color(link_dot, lv_color_hex(linked ? COL_AMBER : COL_DOT_OFF),
                              LV_PART_MAIN);

    uint8_t own = zmk_battery_state_of_charge();
    lv_label_set_text_fmt(own_batt_label, "%d%%", own);

    if (!linked) {
        lv_label_set_text(kb_name_label, "NO LINK");
        lv_label_set_text(layer_label, "");
        lv_label_set_text(batt_pct_label, "");
        lv_obj_set_width(batt_gauge_fg, 0);
        lv_label_set_text(row_name_label, "KEYBOARD");
        lv_obj_set_style_text_color(row_name_label, lv_color_hex(COL_MUTE), LV_PART_MAIN);
        lv_label_set_text(row_pct_label, "");
        for (int i = 0; i < LAYER_COUNT; i++) {
            lv_obj_set_style_bg_color(layer_dots[i], lv_color_hex(COL_DOT_OFF), LV_PART_MAIN);
        }
        return;
    }

    lv_label_set_text(kb_name_label, "KEYBOARD");
    uint8_t layer = st.layer < LAYER_COUNT ? st.layer : 0;
    uint32_t lcol = layer_colors[layer];
    lv_label_set_text_fmt(layer_label, "LAYER %d", st.layer);
    lv_obj_set_style_text_color(layer_label, lv_color_hex(lcol), LV_PART_MAIN);
    for (int i = 0; i < LAYER_COUNT; i++) {
        lv_obj_set_style_bg_color(layer_dots[i],
                                  lv_color_hex(i == layer ? lcol : COL_DOT_OFF), LV_PART_MAIN);
    }

    if (st.battery == 0xFF) {
        lv_label_set_text(batt_pct_label, "--%");
        lv_obj_set_width(batt_gauge_fg, 0);
        lv_label_set_text(row_pct_label, "--%");
    } else {
        lv_label_set_text_fmt(batt_pct_label, "%d%%", st.battery);
        lv_obj_set_width(batt_gauge_fg, 208 * st.battery / 100);
        lv_obj_set_style_bg_color(batt_gauge_fg,
                                  lv_color_hex(st.battery <= 20 ? COL_ALERT : 0xE8E8EA),
                                  LV_PART_MAIN);
        lv_label_set_text_fmt(row_pct_label, "%d%%", st.battery);
    }
    lv_obj_set_style_text_color(row_name_label, lv_color_white(), LV_PART_MAIN);
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    /* Square stage 240x240, centered: survives panel rotation. */
    lv_obj_t *stage = lv_obj_create(screen);
    lv_obj_set_size(stage, 240, 240);
    lv_obj_center(stage);
    lv_obj_set_style_bg_color(stage, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(stage, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(stage, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(stage, 0, LV_PART_MAIN);
    lv_obj_clear_flag(stage, LV_OBJ_FLAG_SCROLLABLE);

    /* Top row: brand / own battery / link state. */
    lv_obj_t *brand = make_label(stage, &lv_font_montserrat_12, COL_MUTE, 16, 10);
    lv_label_set_text(brand, "NAPE DECK");
    lv_obj_set_style_text_letter_space(brand, 3, LV_PART_MAIN);

    own_batt_label = make_label(stage, &lv_font_montserrat_12, COL_SUB, 150, 10);
    lv_label_set_text(own_batt_label, "--%");

    link_dot = make_dot(stage, 196, 13, 6, COL_DOT_OFF);
    lv_obj_t *esb = make_label(stage, &lv_font_montserrat_12, COL_SUB, 206, 10);
    lv_label_set_text(esb, "ESB");

    /* Hero: keyboard name + layer. */
    kb_name_label = make_label(stage, &lv_font_montserrat_28, 0xFFFFFF, 16, 34);
    lv_label_set_text(kb_name_label, "NO LINK");

    layer_label = make_label(stage, &lv_font_montserrat_14, 0xFFFFFF, 16, 74);
    for (int i = 0; i < LAYER_COUNT; i++) {
        layer_dots[i] = make_dot(stage, 96 + i * 9, 79, 5, COL_DOT_OFF);
    }

    /* Battery: percent + hairline gauge. */
    batt_pct_label = make_label(stage, &lv_font_montserrat_20, 0xFFFFFF, 16, 100);
    lv_obj_t *gauge_bg = lv_obj_create(stage);
    lv_obj_set_size(gauge_bg, 208, 3);
    lv_obj_set_pos(gauge_bg, 16, 128);
    lv_obj_set_style_radius(gauge_bg, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(gauge_bg, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(gauge_bg, lv_color_hex(COL_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(gauge_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(gauge_bg, LV_OBJ_FLAG_SCROLLABLE);

    batt_gauge_fg = lv_obj_create(stage);
    lv_obj_set_size(batt_gauge_fg, 0, 3);
    lv_obj_set_pos(batt_gauge_fg, 16, 128);
    lv_obj_set_style_radius(batt_gauge_fg, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(batt_gauge_fg, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(batt_gauge_fg, lv_color_hex(0xE8E8EA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(batt_gauge_fg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(batt_gauge_fg, LV_OBJ_FLAG_SCROLLABLE);

    /* Hairline + keyboards section. */
    lv_obj_t *hr = lv_obj_create(stage);
    lv_obj_set_size(hr, 240, 1);
    lv_obj_set_pos(hr, 0, 143);
    lv_obj_set_style_border_width(hr, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hr, lv_color_hex(COL_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(hr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(hr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *sect = make_label(stage, &lv_font_montserrat_12, COL_MUTE, 16, 154);
    lv_label_set_text(sect, "KEYBOARDS");
    lv_obj_set_style_text_letter_space(sect, 3, LV_PART_MAIN);

    row_radio = lv_obj_create(stage);
    lv_obj_set_size(row_radio, 9, 9);
    lv_obj_set_pos(row_radio, 16, 182);
    lv_obj_set_style_radius(row_radio, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(row_radio, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(row_radio, lv_color_hex(COL_AMBER), LV_PART_MAIN);
    lv_obj_set_style_bg_color(row_radio, lv_color_hex(COL_AMBER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(row_radio, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(row_radio, LV_OBJ_FLAG_SCROLLABLE);

    row_name_label = make_label(stage, &lv_font_montserrat_14, 0xFFFFFF, 34, 178);
    lv_label_set_text(row_name_label, "KEYBOARD");
    row_pct_label = make_label(stage, &lv_font_montserrat_12, COL_SUB, 196, 179);

    lv_timer_create(refresh_cb, 500, NULL);
    refresh_cb(NULL);

    return screen;
}
