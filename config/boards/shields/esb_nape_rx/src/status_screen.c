/*
 * Nape Deck screen — Step 2 multi-page touch UI.
 *
 * Design language (approved v2, kept): pure black background, 240x240
 * square stage centered in the panel, white/gray text only, amber
 * reserved for "active" markers, per-layer colors for the layer name
 * and dots, true neutral greys in the link-lost state.
 *
 * Typography floor: no informational text below 16 px. On this
 * 240 px-wide panel Montserrat only renders cleanly from ~20 px up
 * (14 is borderline, 12 smears — hardware-verified 2026-11-04 with
 * the test pattern: shapes/lines/bars are pixel-perfect, only small
 * antialiased text was soft), so info text is 20 px where the layout
 * allows and 16 px where it does not (tab labels, battery percents,
 * the top-row link state). All sizes route through the FONT_16/20/28
 * macros below: Deck builds use custom-converted Montserrat
 * SemiBold (src/fonts/, CONFIG_ESB_NAPE_RX_CUSTOM_FONT) since the
 * stock Medium smears even at 20/28, just less visibly.
 *
 * Pages (containers created once at init, shown/hidden — never
 * created or destroyed on the refresh tick):
 *   STATUS    the original home screen (active keyboard: name,
 *             layer, battery gauge, link state, own battery)
 *   KEYBOARDS 4 registry slot rows; tap a row for its detail page
 *   DETAIL    per-slot state + SWITCH / DELETE (or REGISTER when
 *             the slot is empty)
 *   CONFIRM   delete confirmation
 *   REGISTER  pairing window progress (searching / candidate / done);
 *             on done the new slot is activated immediately and the
 *             UI lands on STATUS after a short dwell
 * A bottom tab bar (STATUS | KEYBOARDS, >= 40 px tall targets) is
 * always visible. All dynamic text updates happen in the 500 ms
 * refresh; page/tab state lives in static vars so refresh cycles
 * preserve it. On builds without a pointer input (plain
 * esb_nape_rx) the buttons simply are not clickable.
 *
 * Data: the ESB HID registry / status store from the module
 * (<esb_hid.h>, <esb_hid_status.h>). The STATUS page follows the
 * ACTIVE slot; its big name is the slot's registry name, falling
 * back to the status frame's name, then "KEYBOARD".
 */

#include <stdarg.h>
#include <string.h>

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <esb_hid.h>
#include <esb_hid_status.h>

LOG_MODULE_REGISTER(status_screen, LOG_LEVEL_INF);
#include <zmk/battery.h>
#include <zmk/display/status_screen.h>

/* UI fonts: the three sizes the layout uses, routed through one
 * place so the face can be swapped with a single conf change.
 * With CONFIG_ESB_NAPE_RX_CUSTOM_FONT (the Deck builds) these are
 * the custom-converted Montserrat SemiBold fonts in src/fonts/ —
 * heavier strokes, because the stock Medium's antialiasing smears
 * on this panel at every size. The SemiBold smeared the same way
 * at bpp 4 and bpp 8 on hardware; partial-coverage pixels come
 * out colored on this panel, and the bpp 1 conversion (no
 * intermediate alpha steps at all) was hardware-verified clean
 * on 2026-11-06. So CONFIG_ESB_NAPE_RX_FONT_BPP1 (on top of the
 * custom-font switch) sits at the top of the ladder, above
 * CONFIG_ESB_NAPE_RX_FONT_BPP8 and the bpp 4 conversion.
 * Without the custom-font switch (plain esb_nape_rx), the stock
 * LVGL Montserrat is used exactly as before. */
#if defined(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
#include "fonts/fonts_custom.h"
#if defined(CONFIG_ESB_NAPE_RX_FONT_BPP1)
#define FONT_16 (&lv_font_montserrat_sb1_16)
#define FONT_20 (&lv_font_montserrat_sb1_20)
#define FONT_28 (&lv_font_montserrat_sb1_28)
#elif defined(CONFIG_ESB_NAPE_RX_FONT_BPP8)
#define FONT_16 (&lv_font_montserrat_sb8_16)
#define FONT_20 (&lv_font_montserrat_sb8_20)
#define FONT_28 (&lv_font_montserrat_sb8_28)
#else
#define FONT_16 (&lv_font_montserrat_sb_16)
#define FONT_20 (&lv_font_montserrat_sb_20)
#define FONT_28 (&lv_font_montserrat_sb_28)
#endif
#else
#define FONT_16 (&lv_font_montserrat_16)
#define FONT_20 (&lv_font_montserrat_20)
#define FONT_28 (&lv_font_montserrat_28)
#endif

#define COL_SUB 0x9A9AA0
#define COL_MUTE 0x5C5C60
#define COL_LINE 0x26262A
#define COL_DOT_OFF 0x3A3A3E
#define COL_AMBER 0xFFB340
#define COL_ALERT 0xFF453A
#define COL_GREEN 0x32D74B

/* Link-lost state colors: true neutral greys (R=G=B). The shared
 * palette greys above are slightly blue, which reads as purple on
 * this panel once the whole screen is greyed out, so the lost state
 * uses its own values at similar luminance. */
#define COL_LOST_MAIN 0x8C8C8C
#define COL_LOST_MUTE 0x5A5A5A
#define COL_LOST_DOT 0x6E6E6E

/* Top-right link indicator positions. The wider "NO LINK" text of
 * the link-lost state would run off the 240 px stage at the normal
 * position, so in that state dot + label shift left together (and
 * shift back when the link returns). Nothing else moves. */
#define LINK_DOT_X 180
#define LINK_LABEL_X 188
#define LINK_DOT_X_LOST 158
#define LINK_LABEL_X_LOST 168
#define LINK_DOT_Y 16

/* Content pages occupy the stage above the tab bar. */
#define PAGE_W 240
#define PAGE_H 200
#define TAB_BAR_Y 200
#define TAB_BAR_H 40

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

/* ---- Page state ---- */
enum deck_page {
    PAGE_STATUS,
    PAGE_KEYBOARDS,
    PAGE_DETAIL,
    PAGE_CONFIRM,
    PAGE_REGISTER,
};

static enum deck_page cur_page = PAGE_STATUS;
static uint8_t detail_slot;
static int64_t register_done_ms;   /* when pairing DONE was first seen */
static bool register_was_active;   /* pairing left IDLE on this page */
static bool register_switched;     /* auto-switch to the new slot attempted */
static bool register_switch_ok;    /* auto-switch to the new slot succeeded */

/* ---- Widgets ---- */
static lv_obj_t *pages[5];
static lv_obj_t *tab_btns[2];
static lv_obj_t *tab_labels[2];

/* STATUS page */
static lv_obj_t *link_dot;
static lv_obj_t *link_label;
static lv_obj_t *own_batt_label;
static lv_obj_t *kb_name_label;
static lv_obj_t *layer_label;
static lv_obj_t *layer_dots[LAYER_COUNT];
static lv_obj_t *batt_pct_label;
static lv_obj_t *batt_gauge_fg;
static lv_obj_t *row_radio;
static lv_obj_t *row_name_label;
static lv_obj_t *row_pct_label;

/* KEYBOARDS page */
static lv_obj_t *slot_dot[ESB_HID_SLOT_COUNT];
static lv_obj_t *slot_name_label[ESB_HID_SLOT_COUNT];
static lv_obj_t *slot_pct_label[ESB_HID_SLOT_COUNT];
static lv_obj_t *slot_active_label[ESB_HID_SLOT_COUNT];

/* DETAIL page */
static lv_obj_t *detail_name_label;
static lv_obj_t *detail_batt_label;
static lv_obj_t *detail_layer_label;
static lv_obj_t *detail_link_label;
static lv_obj_t *detail_output_label;
static lv_obj_t *detail_switch_btn;
static lv_obj_t *detail_delete_btn;
static lv_obj_t *detail_register_btn;

/* CONFIRM page */
static lv_obj_t *confirm_title_label;

/* REGISTER page */
static lv_obj_t *register_status_label;
static lv_obj_t *register_confirm_btn;

/* ---- Change-guarded setters for the 500 ms refresh ----
 * LVGL setters invalidate the object's area even when the new value
 * is identical to the one already applied, so unconditional refresh
 * writes keep the panel repainting the same pixels twice a second
 * instead of leaving a static screen untouched. (The Prospector
 * Scanner firmware never repaints a static screen — it is event
 * driven — and does not show this shimmer on the same panel.) These
 * helpers make identical writes a no-op, for label text as well as
 * for the style/position writes: the style helpers below remember
 * the last value applied per (object, part, property), so a write
 * whose value actually changed — the link-lost grey recolor, a new
 * layer color, the ACTIVE marker — still goes through on the very
 * tick it happens, and only the identical re-application is
 * skipped. */
static void set_label_text_if_changed(lv_obj_t *label, const char *text) {
    const char *cur = lv_label_get_text(label);

    if (cur == NULL || strcmp(cur, text) != 0) {
        lv_label_set_text(label, text);
    }
}

static void set_label_text_fmt_if_changed(lv_obj_t *label, const char *fmt, ...) {
    static char buf[64];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    set_label_text_if_changed(label, buf);
}

/* The battery gauge is a plain object whose width encodes the
 * charge; cache the last applied width so identical widths do not
 * re-invalidate it either. */
static lv_coord_t batt_gauge_applied_w = -1;

static void set_gauge_width_if_changed(lv_obj_t *obj, lv_coord_t w) {
    if (w != batt_gauge_applied_w) {
        batt_gauge_applied_w = w;
        lv_obj_set_width(obj, w);
    }
}

/* ---- Change-guarded style / position setters ----
 * A tiny cache of the last value applied per (object, part,
 * property). All screen objects live for the whole run (pages are
 * created once, never destroyed), so keying on the object pointer
 * is safe. When the cache fills up the helpers degrade to applying
 * unconditionally, never to skipping a needed write. */

enum style_prop_kind {
    STYLE_PROP_TEXT_COLOR,
    STYLE_PROP_BG_COLOR,
    STYLE_PROP_BORDER_COLOR,
    STYLE_PROP_TEXT_FONT,
    STYLE_PROP_POS,
};

struct style_prop_entry {
    const lv_obj_t *obj;
    lv_part_t part;
    uint8_t kind;   /* enum style_prop_kind */
    uint32_t value; /* color hex, or the font pointer */
    lv_coord_t x;   /* STYLE_PROP_POS only */
    lv_coord_t y;   /* STYLE_PROP_POS only */
};

#define STYLE_PROP_CACHE_SIZE 48
static struct style_prop_entry style_prop_cache[STYLE_PROP_CACHE_SIZE];

/* True when exactly this value was already applied to (obj, part,
 * kind) and the setter can be skipped; otherwise records the value
 * and returns false so the caller applies it now. */
static bool style_prop_already_applied(const lv_obj_t *obj, lv_part_t part,
                                       enum style_prop_kind kind, uint32_t value, lv_coord_t x,
                                       lv_coord_t y) {
    struct style_prop_entry *empty = NULL;

    for (int i = 0; i < STYLE_PROP_CACHE_SIZE; i++) {
        struct style_prop_entry *e = &style_prop_cache[i];

        if (e->obj == NULL) {
            if (empty == NULL) {
                empty = e;
            }
            continue;
        }
        if (e->obj == obj && e->part == part && e->kind == (uint8_t)kind) {
            if (e->value == value && e->x == x && e->y == y) {
                return true;
            }
            e->value = value;
            e->x = x;
            e->y = y;
            return false;
        }
    }
    if (empty != NULL) {
        empty->obj = obj;
        empty->part = part;
        empty->kind = (uint8_t)kind;
        empty->value = value;
        empty->x = x;
        empty->y = y;
    }
    return false;
}

static void set_style_text_color_if_changed(lv_obj_t *obj, lv_part_t part, uint32_t hex) {
    if (!style_prop_already_applied(obj, part, STYLE_PROP_TEXT_COLOR, hex, 0, 0)) {
        lv_obj_set_style_text_color(obj, lv_color_hex(hex), part);
    }
}

static void set_style_bg_color_if_changed(lv_obj_t *obj, lv_part_t part, uint32_t hex) {
    if (!style_prop_already_applied(obj, part, STYLE_PROP_BG_COLOR, hex, 0, 0)) {
        lv_obj_set_style_bg_color(obj, lv_color_hex(hex), part);
    }
}

static void set_style_border_color_if_changed(lv_obj_t *obj, lv_part_t part, uint32_t hex) {
    if (!style_prop_already_applied(obj, part, STYLE_PROP_BORDER_COLOR, hex, 0, 0)) {
        lv_obj_set_style_border_color(obj, lv_color_hex(hex), part);
    }
}

static void set_style_text_font_if_changed(lv_obj_t *obj, lv_part_t part, const lv_font_t *font) {
    if (!style_prop_already_applied(obj, part, STYLE_PROP_TEXT_FONT, (uint32_t)(uintptr_t)font, 0,
                                    0)) {
        lv_obj_set_style_text_font(obj, font, part);
    }
}

static void set_pos_if_changed(lv_obj_t *obj, lv_coord_t x, lv_coord_t y) {
    if (!style_prop_already_applied(obj, LV_PART_MAIN, STYLE_PROP_POS, 0, x, y)) {
        lv_obj_set_pos(obj, x, y);
    }
}

/* add_flag / clear_flag for HIDDEN likewise touch the object even
 * when the flag already has the wanted state. */
static void set_hidden_if_changed(lv_obj_t *obj, bool hidden) {
    if (hidden != lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) {
        if (hidden) {
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

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

/* A button with explicit styles: this build's default theme renders
 * a plain button (nearly) invisible (learned in the touch test), so
 * every button sets bg / text colors itself. */
static lv_obj_t *make_button(lv_obj_t *parent, const char *text, int x, int y, int w, int h,
                             uint32_t bg, uint32_t fg, lv_event_cb_t cb, void *user_data) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, user_data);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_text_font(label, FONT_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(fg), LV_PART_MAIN);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return btn;
}

static void style_button(lv_obj_t *btn, uint32_t bg, uint32_t fg) {
    set_style_bg_color_if_changed(btn, LV_PART_MAIN, bg);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    if (label != NULL) {
        set_style_text_color_if_changed(label, LV_PART_MAIN, fg);
    }
}

/* ---- Page switching ---- */

static void show_page(enum deck_page page) {
    cur_page = page;
    for (int i = 0; i < 5; i++) {
        if (i == (int)page) {
            lv_obj_clear_flag(pages[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(pages[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    /* The tab bar highlights the section the page belongs to:
     * DETAIL / CONFIRM / REGISTER are reached from KEYBOARDS. */
    int active_tab = (page == PAGE_STATUS) ? 0 : 1;
    for (int i = 0; i < 2; i++) {
        bool active = (i == active_tab);
        lv_obj_set_style_bg_color(tab_btns[i], lv_color_hex(active ? COL_LINE : 0x000000),
                                  LV_PART_MAIN);
        lv_obj_set_style_text_color(tab_labels[i],
                                    lv_color_hex(active ? 0xFFFFFF : COL_SUB), LV_PART_MAIN);
    }
}

static void tab_status_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    show_page(PAGE_STATUS);
}

static void tab_keyboards_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    show_page(PAGE_KEYBOARDS);
}

static void slot_row_cb(lv_event_t *e) {
    detail_slot = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    show_page(PAGE_DETAIL);
}

static void detail_switch_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    esb_hid_rx_set_active_slot(detail_slot);
    /* Land on STATUS so the user sees the new active device. */
    show_page(PAGE_STATUS);
}

static void detail_delete_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    show_page(PAGE_CONFIRM);
}

static void detail_back_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    show_page(PAGE_KEYBOARDS);
}

static void detail_register_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    register_done_ms = 0;
    register_was_active = false;
    register_switched = false;
    register_switch_ok = false;
    esb_hid_pairing_open(detail_slot);
    show_page(PAGE_REGISTER);
}

static void confirm_delete_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    esb_hid_rx_delete_slot(detail_slot);
    show_page(PAGE_KEYBOARDS);
}

static void confirm_cancel_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    show_page(PAGE_DETAIL);
}

static void register_confirm_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    struct esb_hid_pairing_status ps;
    if (esb_hid_pairing_get_status(&ps) == 0 && ps.state >= ESB_HID_PAIRING_CANDIDATE &&
        ps.state != ESB_HID_PAIRING_DONE) {
        esb_hid_pairing_confirm();
    }
}

static void register_cancel_cb(lv_event_t *e) {
    ARG_UNUSED(e);
    esb_hid_pairing_cancel();
    show_page(PAGE_DETAIL);
}

/* ---- Name resolution for the STATUS hero ---- */

static const char *resolve_active_name(uint8_t slot, const struct esb_hid_device_status *st) {
    static char buf[ESB_HID_NAME_MAX + 1];
    struct esb_hid_slot_info info;

    if (esb_hid_rx_get_slot_info(slot, &info) == 0 && info.used && info.name[0] != '\0') {
        strncpy(buf, info.name, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    if (st != NULL && st->name[0] != '\0') {
        strncpy(buf, st->name, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    return "KEYBOARD";
}

static const char *output_name(uint8_t output) {
    switch (output) {
    case ESB_HID_STATUS_OUTPUT_USB:
        return "USB";
    case ESB_HID_STATUS_OUTPUT_BLE:
        return "BLE";
    case ESB_HID_STATUS_OUTPUT_NONE:
        return "NONE";
    default:
        return "ESB";
    }
}

/* ---- Refresh: STATUS page (the original rendering, now following
 *      the active slot instead of device 0) ---- */

static void refresh_status_page(void) {
    uint8_t active = esb_hid_rx_get_active_slot();
    struct esb_hid_device_status st;
    bool linked = esb_hid_get_device_status(active, &st) && st.valid;
    /* A keyboard that was seen before but has sent no frame of any
     * type for longer than the timeout is gone: keep showing its
     * last known state, greyed out (below), instead of presenting
     * it as live. */
    bool link_lost = linked && (k_uptime_get() - st.last_seen_ms > ESB_HID_LINK_LOST_TIMEOUT_MS);
    const char *kb_name = linked ? resolve_active_name(active, &st) : "KEYBOARD";

    uint8_t own = zmk_battery_state_of_charge();
    set_label_text_fmt_if_changed(own_batt_label, "%d%%", own);
    set_style_text_color_if_changed(own_batt_label, LV_PART_MAIN,
                                    link_lost ? COL_LOST_MUTE : COL_SUB);

    if (!linked) {
        set_style_bg_color_if_changed(link_dot, LV_PART_MAIN, COL_DOT_OFF);
        set_label_text_if_changed(link_label, "ESB");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, COL_SUB);
        set_label_text_if_changed(kb_name_label, "NO LINK");
        set_label_text_if_changed(layer_label, "");
        set_label_text_if_changed(batt_pct_label, "");
        set_gauge_width_if_changed(batt_gauge_fg, 0);
        set_label_text_if_changed(row_name_label, "KEYBOARD");
        set_style_text_color_if_changed(row_name_label, LV_PART_MAIN, COL_MUTE);
        set_label_text_if_changed(row_pct_label, "");
        for (int i = 0; i < LAYER_COUNT; i++) {
            set_style_bg_color_if_changed(layer_dots[i], LV_PART_MAIN, COL_DOT_OFF);
        }
        return;
    }

    if (link_lost) {
        /* Last known state, all in muted greys; only the link area
         * changes its label. */
        set_pos_if_changed(link_dot, LINK_DOT_X_LOST, LINK_DOT_Y);
        set_pos_if_changed(link_label, LINK_LABEL_X_LOST, 10);
        set_style_bg_color_if_changed(link_dot, LV_PART_MAIN, COL_LOST_DOT);
        set_label_text_if_changed(link_label, "NO LINK");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, COL_LOST_MUTE);

        set_label_text_if_changed(kb_name_label, kb_name);
        set_style_text_color_if_changed(kb_name_label, LV_PART_MAIN, COL_LOST_MAIN);

        if (st.layer_name[0] != '\0') {
            set_label_text_if_changed(layer_label, st.layer_name);
        } else {
            set_label_text_fmt_if_changed(layer_label, "LAYER %d", st.layer);
        }
        set_style_text_color_if_changed(layer_label, LV_PART_MAIN, COL_LOST_MUTE);
        for (int i = 0; i < LAYER_COUNT; i++) {
            set_style_bg_color_if_changed(layer_dots[i], LV_PART_MAIN, COL_LOST_DOT);
        }

        set_style_text_color_if_changed(batt_pct_label, LV_PART_MAIN, COL_LOST_MUTE);
        set_style_text_color_if_changed(row_pct_label, LV_PART_MAIN, COL_LOST_MUTE);
        if (st.battery == 0xFF) {
            set_label_text_if_changed(batt_pct_label, "--%");
            set_gauge_width_if_changed(batt_gauge_fg, 0);
            set_label_text_if_changed(row_pct_label, "--%");
        } else {
            set_label_text_fmt_if_changed(batt_pct_label, "%d%%", st.battery);
            set_gauge_width_if_changed(batt_gauge_fg, 208 * st.battery / 100);
            set_style_bg_color_if_changed(batt_gauge_fg, LV_PART_MAIN, COL_LOST_DOT);
            set_label_text_fmt_if_changed(row_pct_label, "%d%%", st.battery);
        }

        set_style_border_color_if_changed(row_radio, LV_PART_MAIN, COL_LOST_MUTE);
        set_style_bg_color_if_changed(row_radio, LV_PART_MAIN, COL_LOST_DOT);
        set_label_text_if_changed(row_name_label, kb_name);
        set_style_text_color_if_changed(row_name_label, LV_PART_MAIN, COL_LOST_MUTE);
        return;
    }

    /* Link alive: undo any link-lost styling before the normal
     * rendering below. */
    set_pos_if_changed(link_dot, LINK_DOT_X, LINK_DOT_Y);
    set_pos_if_changed(link_label, LINK_LABEL_X, 10);
    set_style_text_color_if_changed(kb_name_label, LV_PART_MAIN, 0xFFFFFF);
    set_style_text_color_if_changed(batt_pct_label, LV_PART_MAIN, 0xFFFFFF);
    set_style_text_color_if_changed(row_pct_label, LV_PART_MAIN, COL_SUB);

    set_label_text_if_changed(kb_name_label, kb_name);

    /* Output-mode indication. Amber stays reserved for "the ESB link
     * is carrying the keyboard's output" (link dot + list-row ring).
     * When the keyboard sends its HID output over USB/BLE instead, the
     * status frames still arrive over ESB, so layer/battery keep
     * updating, but the markers drop to gray and the label names the
     * active output. */
    bool via_esb = st.output == ESB_HID_STATUS_OUTPUT_ESB;
    set_style_bg_color_if_changed(link_dot, LV_PART_MAIN, via_esb ? COL_AMBER : COL_SUB);
    set_style_border_color_if_changed(row_radio, LV_PART_MAIN,
                                      via_esb ? COL_AMBER : COL_MUTE);
    set_style_bg_color_if_changed(row_radio, LV_PART_MAIN,
                                  via_esb ? COL_AMBER : COL_DOT_OFF);
    switch (st.output) {
    case ESB_HID_STATUS_OUTPUT_USB:
        set_label_text_if_changed(link_label, "USB");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, 0xFFFFFF);
        break;
    case ESB_HID_STATUS_OUTPUT_BLE:
        set_label_text_if_changed(link_label, "BLE");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, 0xFFFFFF);
        break;
    case ESB_HID_STATUS_OUTPUT_NONE:
        set_label_text_if_changed(link_label, "NONE");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, COL_MUTE);
        break;
    default: /* ESB_HID_STATUS_OUTPUT_ESB */
        set_label_text_if_changed(link_label, "ESB");
        set_style_text_color_if_changed(link_label, LV_PART_MAIN, COL_SUB);
        break;
    }

    uint8_t layer = st.layer < LAYER_COUNT ? st.layer : 0;
    uint32_t lcol = layer_colors[layer];
    if (st.layer_name[0] != '\0') {
        set_label_text_if_changed(layer_label, st.layer_name);
    } else {
        set_label_text_fmt_if_changed(layer_label, "LAYER %d", st.layer);
    }
    set_style_text_color_if_changed(layer_label, LV_PART_MAIN, lcol);
    for (int i = 0; i < LAYER_COUNT; i++) {
        set_style_bg_color_if_changed(layer_dots[i], LV_PART_MAIN,
                                      i == layer ? lcol : COL_DOT_OFF);
    }

    if (st.battery == 0xFF) {
        set_label_text_if_changed(batt_pct_label, "--%");
        set_gauge_width_if_changed(batt_gauge_fg, 0);
        set_label_text_if_changed(row_pct_label, "--%");
    } else {
        set_label_text_fmt_if_changed(batt_pct_label, "%d%%", st.battery);
        set_gauge_width_if_changed(batt_gauge_fg, 208 * st.battery / 100);
        set_style_bg_color_if_changed(batt_gauge_fg, LV_PART_MAIN,
                                      st.battery <= 20 ? COL_ALERT : 0xE8E8EA);
        set_label_text_fmt_if_changed(row_pct_label, "%d%%", st.battery);
    }
    set_label_text_if_changed(row_name_label, kb_name);
    set_style_text_color_if_changed(row_name_label, LV_PART_MAIN, 0xFFFFFF);
}

/* ---- Refresh: KEYBOARDS page ---- */

static void refresh_keyboards_page(void) {
    uint8_t active = esb_hid_rx_get_active_slot();

    for (int i = 0; i < ESB_HID_SLOT_COUNT; i++) {
        struct esb_hid_slot_info info;
        bool ok = esb_hid_rx_get_slot_info(i, &info) == 0 && info.used;

        set_style_bg_color_if_changed(slot_dot[i], LV_PART_MAIN,
                                      ok && info.link_up ? COL_GREEN : COL_DOT_OFF);
        if (ok) {
            set_label_text_if_changed(slot_name_label[i],
                                      info.name[0] != '\0' ? info.name : "KEYBOARD");
            set_style_text_color_if_changed(slot_name_label[i], LV_PART_MAIN, 0xFFFFFF);
            if (info.battery == 0xFF) {
                set_label_text_if_changed(slot_pct_label[i], "--");
            } else {
                set_label_text_fmt_if_changed(slot_pct_label[i], "%d%%", info.battery);
            }
        } else {
            set_label_text_if_changed(slot_name_label[i], "EMPTY");
            set_style_text_color_if_changed(slot_name_label[i], LV_PART_MAIN, COL_MUTE);
            set_label_text_if_changed(slot_pct_label[i], "--");
        }
        set_hidden_if_changed(slot_active_label[i], !(i == active && ok));
    }
}

/* ---- Refresh: DETAIL page ---- */

static void refresh_detail_page(void) {
    struct esb_hid_slot_info info;
    bool used = esb_hid_rx_get_slot_info(detail_slot, &info) == 0 && info.used;
    bool is_active = detail_slot == esb_hid_rx_get_active_slot();

    if (!used) {
        set_label_text_if_changed(detail_name_label, "EMPTY SLOT");
        set_style_text_font_if_changed(detail_name_label, LV_PART_MAIN, FONT_20);
        set_label_text_if_changed(detail_batt_label, "Battery --");
        set_label_text_if_changed(detail_layer_label, "Layer --");
        set_label_text_if_changed(detail_link_label, "--");
        set_label_text_if_changed(detail_output_label, "Output --");
        set_hidden_if_changed(detail_switch_btn, true);
        set_hidden_if_changed(detail_delete_btn, true);
        set_hidden_if_changed(detail_register_btn, false);
        return;
    }

    set_label_text_if_changed(detail_name_label, info.name[0] != '\0' ? info.name : "KEYBOARD");
    set_style_text_font_if_changed(detail_name_label, LV_PART_MAIN, FONT_28);
    if (info.battery == 0xFF) {
        set_label_text_if_changed(detail_batt_label, "Battery --");
    } else {
        set_label_text_fmt_if_changed(detail_batt_label, "Battery %d%%", info.battery);
    }
    if (info.status_valid) {
        if (info.layer_name[0] != '\0') {
            set_label_text_if_changed(detail_layer_label, info.layer_name);
        } else {
            set_label_text_fmt_if_changed(detail_layer_label, "Layer %d", info.layer);
        }
        set_label_text_fmt_if_changed(detail_output_label, "Output %s", output_name(info.output));
    } else {
        set_label_text_if_changed(detail_layer_label, "Layer --");
        set_label_text_if_changed(detail_output_label, "Output --");
    }
    set_label_text_if_changed(detail_link_label, info.link_up ? "LINKED" : "NO LINK");
    set_style_text_color_if_changed(detail_link_label, LV_PART_MAIN,
                                    info.link_up ? COL_GREEN : COL_ALERT);

    set_hidden_if_changed(detail_switch_btn, is_active);
    set_hidden_if_changed(detail_delete_btn, false);
    set_hidden_if_changed(detail_register_btn, true);

    set_label_text_fmt_if_changed(confirm_title_label, "Delete %s?",
                                  info.name[0] != '\0' ? info.name : "KEYBOARD");
}

/* ---- Refresh: REGISTER page ---- */

static void refresh_register_page(void) {
    struct esb_hid_pairing_status ps;

    if (esb_hid_pairing_get_status(&ps) != 0) {
        set_label_text_if_changed(register_status_label, "Searching...");
        style_button(register_confirm_btn, COL_LINE, COL_MUTE);
        return;
    }

    if (ps.state != ESB_HID_PAIRING_IDLE) {
        register_was_active = true;
    }

    switch (ps.state) {
    case ESB_HID_PAIRING_SEARCHING:
        if (ps.remaining_ms > 0) {
            set_label_text_fmt_if_changed(register_status_label, "Searching... %ds",
                                          (int)(ps.remaining_ms / 1000));
        } else {
            set_label_text_if_changed(register_status_label, "Searching...");
        }
        style_button(register_confirm_btn, COL_LINE, COL_MUTE);
        break;
    case ESB_HID_PAIRING_CANDIDATE:
        set_label_text_fmt_if_changed(register_status_label, "Found: %s", ps.candidate_name);
        style_button(register_confirm_btn, COL_AMBER, 0x000000);
        break;
    case ESB_HID_PAIRING_ASSIGNING:
        set_label_text_if_changed(register_status_label, "Registering...");
        style_button(register_confirm_btn, COL_LINE, COL_MUTE);
        break;
    case ESB_HID_PAIRING_DONE: {
        if (!register_switched) {
            /* Registration complete: activate the new keyboard right
             * away so it can be used without a separate SWITCH tap.
             * Guarded so this runs exactly once per registration. */
            int err = esb_hid_rx_set_active_slot(ps.target_slot);

            register_switched = true;
            register_switch_ok = (err == 0);
            if (err != 0) {
                LOG_WRN("auto-switch to registered slot %u failed (%d)", ps.target_slot, err);
            }
        }
        set_label_text_if_changed(register_status_label,
                                  register_switch_ok ? "Registered - ACTIVE" : "Registered");
        style_button(register_confirm_btn, COL_LINE, COL_MUTE);
        if (register_done_ms == 0) {
            register_done_ms = k_uptime_get();
        } else if (k_uptime_get() - register_done_ms > 1500) {
            /* Clear the DONE state. Land on STATUS now that the new
             * slot is active; if the switch failed, fall back to the
             * slot's detail page as before. */
            enum deck_page dest = register_switch_ok ? PAGE_STATUS : PAGE_DETAIL;

            register_done_ms = 0;
            register_was_active = false;
            register_switched = false;
            register_switch_ok = false;
            esb_hid_pairing_cancel();
            show_page(dest);
        }
        break;
    }
    default: /* IDLE: the window timed out while on this page */
        if (register_was_active) {
            register_was_active = false;
            show_page(PAGE_DETAIL);
        } else {
            set_label_text_if_changed(register_status_label, "Searching...");
            style_button(register_confirm_btn, COL_LINE, COL_MUTE);
        }
        break;
    }
}

static void refresh_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    refresh_status_page();
    switch (cur_page) {
    case PAGE_KEYBOARDS:
        refresh_keyboards_page();
        break;
    case PAGE_DETAIL:
    case PAGE_CONFIRM:
        refresh_detail_page();
        break;
    case PAGE_REGISTER:
        refresh_register_page();
        break;
    default:
        break;
    }
}

/* ---- Screen construction ---- */

static lv_obj_t *make_page(lv_obj_t *stage) {
    lv_obj_t *page = lv_obj_create(stage);
    lv_obj_set_size(page, PAGE_W, PAGE_H);
    lv_obj_set_pos(page, 0, 0);
    lv_obj_set_style_bg_color(page, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(page, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(page, 0, LV_PART_MAIN);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    return page;
}

static void build_status_page(lv_obj_t *page) {
    /* Top row: brand / own battery / link state. */
    lv_obj_t *brand = make_label(page, FONT_16, 0xE8E8EA, 16, 10);
    lv_label_set_text(brand, "NAPE DECK");
    lv_obj_set_style_text_letter_space(brand, 2, LV_PART_MAIN);

    own_batt_label = make_label(page, FONT_16, 0xFFFFFF, 134, 10);
    lv_label_set_text(own_batt_label, "--%");

    link_dot = make_dot(page, LINK_DOT_X, LINK_DOT_Y, 6, COL_DOT_OFF);
    link_label = make_label(page, FONT_16, 0xFFFFFF, LINK_LABEL_X, 10);
    lv_label_set_text(link_label, "ESB");

    /* Hero: keyboard name + layer. */
    kb_name_label = make_label(page, FONT_28, 0xFFFFFF, 16, 34);
    lv_label_set_text(kb_name_label, "NO LINK");

    layer_label = make_label(page, FONT_16, 0xFFFFFF, 16, 74);
    for (int i = 0; i < LAYER_COUNT; i++) {
        layer_dots[i] = make_dot(page, 104 + i * 9, 80, 5, COL_DOT_OFF);
    }

    /* Battery: percent + hairline gauge. */
    batt_pct_label = make_label(page, FONT_20, 0xFFFFFF, 16, 100);
    lv_obj_t *gauge_bg = lv_obj_create(page);
    lv_obj_set_size(gauge_bg, 208, 3);
    lv_obj_set_pos(gauge_bg, 16, 128);
    lv_obj_set_style_radius(gauge_bg, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(gauge_bg, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(gauge_bg, lv_color_hex(COL_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(gauge_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(gauge_bg, LV_OBJ_FLAG_SCROLLABLE);

    batt_gauge_fg = lv_obj_create(page);
    lv_obj_set_size(batt_gauge_fg, 0, 3);
    lv_obj_set_pos(batt_gauge_fg, 16, 128);
    lv_obj_set_style_radius(batt_gauge_fg, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(batt_gauge_fg, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(batt_gauge_fg, lv_color_hex(0xE8E8EA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(batt_gauge_fg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(batt_gauge_fg, LV_OBJ_FLAG_SCROLLABLE);

    /* Hairline + keyboards section. */
    lv_obj_t *hr = lv_obj_create(page);
    lv_obj_set_size(hr, 240, 1);
    lv_obj_set_pos(hr, 0, 143);
    lv_obj_set_style_border_width(hr, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hr, lv_color_hex(COL_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(hr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(hr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *sect = make_label(page, FONT_16, 0xE8E8EA, 16, 154);
    lv_label_set_text(sect, "KEYBOARDS");
    lv_obj_set_style_text_letter_space(sect, 3, LV_PART_MAIN);

    row_radio = lv_obj_create(page);
    lv_obj_set_size(row_radio, 9, 9);
    lv_obj_set_pos(row_radio, 16, 182);
    lv_obj_set_style_radius(row_radio, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(row_radio, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(row_radio, lv_color_hex(COL_AMBER), LV_PART_MAIN);
    lv_obj_set_style_bg_color(row_radio, lv_color_hex(COL_AMBER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(row_radio, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(row_radio, LV_OBJ_FLAG_SCROLLABLE);

    row_name_label = make_label(page, FONT_20, 0xFFFFFF, 34, 175);
    lv_label_set_text(row_name_label, "KEYBOARD");
    row_pct_label = make_label(page, FONT_16, 0xFFFFFF, 196, 178);
}

static void build_keyboards_page(lv_obj_t *page) {
    lv_obj_t *title = make_label(page, FONT_20, 0xFFFFFF, 16, 2);
    lv_label_set_text(title, "KEYBOARDS");
    lv_obj_set_style_text_letter_space(title, 3, LV_PART_MAIN);

    for (int i = 0; i < ESB_HID_SLOT_COUNT; i++) {
        lv_obj_t *row = lv_obj_create(page);
        lv_obj_set_size(row, 208, 44);
        lv_obj_set_pos(row, 16, 24 + i * 44);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(row, slot_row_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);

        slot_dot[i] = make_dot(row, 0, 19, 6, COL_DOT_OFF);
        slot_name_label[i] = make_label(row, FONT_20, 0xFFFFFF, 16, 1);
        lv_label_set_text(slot_name_label[i], "EMPTY");
        slot_active_label[i] = make_label(row, FONT_16, COL_AMBER, 16, 24);
        lv_label_set_text(slot_active_label[i], "ACTIVE");
        slot_pct_label[i] = make_label(row, FONT_16, 0xFFFFFF, 162, 3);
        lv_label_set_text(slot_pct_label[i], "--");
    }
}

static void build_detail_page(lv_obj_t *page) {
    detail_name_label = make_label(page, FONT_28, 0xFFFFFF, 16, 10);
    lv_label_set_text(detail_name_label, "EMPTY SLOT");

    detail_batt_label = make_label(page, FONT_20, 0xFFFFFF, 16, 58);
    detail_layer_label = make_label(page, FONT_20, 0xFFFFFF, 16, 82);
    detail_link_label = make_label(page, FONT_20, 0xFFFFFF, 16, 106);
    detail_output_label = make_label(page, FONT_20, 0xFFFFFF, 16, 130);

    detail_switch_btn = make_button(page, "SWITCH", 16, 158, 70, 36, COL_LINE, 0xFFFFFF,
                                    detail_switch_cb, NULL);
    detail_delete_btn = make_button(page, "DELETE", 90, 158, 70, 36, COL_LINE, COL_ALERT,
                                    detail_delete_cb, NULL);
    detail_register_btn = make_button(page, "REGISTER", 16, 158, 100, 36, COL_AMBER, 0x000000,
                                      detail_register_cb, NULL);
    make_button(page, "BACK", 164, 158, 60, 36, COL_LINE, 0xFFFFFF, detail_back_cb, NULL);
}

static void build_confirm_page(lv_obj_t *page) {
    confirm_title_label = make_label(page, FONT_20, 0xFFFFFF, 16, 16);
    lv_obj_set_width(confirm_title_label, 208);
    lv_label_set_long_mode(confirm_title_label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(confirm_title_label, "Delete?");

    lv_obj_t *warn = make_label(page, FONT_16, 0xE8E8EA, 16, 72);
    lv_label_set_text(warn, "The keyboard must register again to reconnect.");

    make_button(page, "DELETE", 16, 140, 96, 40, COL_ALERT, 0xFFFFFF, confirm_delete_cb, NULL);
    make_button(page, "CANCEL", 128, 140, 96, 40, COL_LINE, 0xFFFFFF, confirm_cancel_cb, NULL);
}

static void build_register_page(lv_obj_t *page) {
    lv_obj_t *title = make_label(page, FONT_20, 0xFFFFFF, 16, 10);
    lv_label_set_text(title, "REGISTER");

    lv_obj_t *hint = make_label(page, FONT_16, 0xE8E8EA, 16, 42);
    lv_obj_set_width(hint, 208);
    lv_label_set_long_mode(hint, LV_LABEL_LONG_WRAP);
    lv_label_set_text(hint, "Press the pairing key on the keyboard.");

    register_status_label = make_label(page, FONT_20, 0xFFFFFF, 16, 90);
    lv_label_set_text(register_status_label, "Searching...");

    register_confirm_btn = make_button(page, "REGISTER", 16, 130, 96, 40, COL_LINE, COL_MUTE,
                                       register_confirm_cb, NULL);
    make_button(page, "CANCEL", 128, 130, 96, 40, COL_LINE, 0xFFFFFF, register_cancel_cb, NULL);
}

static void build_tab_bar(lv_obj_t *stage) {
    lv_obj_t *bar = lv_obj_create(stage);
    lv_obj_set_size(bar, PAGE_W, TAB_BAR_H);
    lv_obj_set_pos(bar, 0, TAB_BAR_Y);
    lv_obj_set_style_bg_color(bar, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *hr = lv_obj_create(bar);
    lv_obj_set_size(hr, PAGE_W, 1);
    lv_obj_set_pos(hr, 0, 0);
    lv_obj_set_style_border_width(hr, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hr, lv_color_hex(COL_LINE), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(hr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(hr, LV_OBJ_FLAG_SCROLLABLE);

    static const char *names[2] = {"STATUS", "KEYBOARDS"};
    lv_event_cb_t cbs[2] = {tab_status_cb, tab_keyboards_cb};

    for (int i = 0; i < 2; i++) {
        lv_obj_t *btn = lv_button_create(bar);
        lv_obj_set_pos(btn, i * 120, 1);
        lv_obj_set_size(btn, 120, TAB_BAR_H - 1);
        lv_obj_set_style_bg_color(btn, lv_color_black(), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_radius(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);
        lv_obj_add_event_cb(btn, cbs[i], LV_EVENT_CLICKED, NULL);
        tab_btns[i] = btn;

        lv_obj_t *label = lv_label_create(btn);
        lv_obj_set_style_text_font(label, FONT_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(COL_SUB), LV_PART_MAIN);
        lv_label_set_text(label, names[i]);
        lv_obj_center(label);
        tab_labels[i] = label;
    }
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

    pages[PAGE_STATUS] = make_page(stage);
    pages[PAGE_KEYBOARDS] = make_page(stage);
    pages[PAGE_DETAIL] = make_page(stage);
    pages[PAGE_CONFIRM] = make_page(stage);
    pages[PAGE_REGISTER] = make_page(stage);

    build_status_page(pages[PAGE_STATUS]);
    build_keyboards_page(pages[PAGE_KEYBOARDS]);
    build_detail_page(pages[PAGE_DETAIL]);
    build_confirm_page(pages[PAGE_CONFIRM]);
    build_register_page(pages[PAGE_REGISTER]);
    build_tab_bar(stage);

    show_page(PAGE_STATUS);

    lv_timer_create(refresh_cb, 500, NULL);
    refresh_cb(NULL);

    return screen;
}
