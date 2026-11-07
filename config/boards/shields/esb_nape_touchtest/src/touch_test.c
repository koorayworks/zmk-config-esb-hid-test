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
 * v11.1 (first hardware verdict): a ROTATE tap wedged the
 * whole UI — frozen frame, dead touch, and none of the
 * instrumented logs ever flushed. The switch is now staged:
 * the panel step runs on the system work queue, the LVGL
 * reframe on the display queue, with log-drain sleeps before
 * each risky call (see the stage handlers for the reasoning).
 *
 * v11.2 (second hardware verdict): the staging worked as a
 * trap. The log trail proved the panel step innocent
 * (display_set_orientation -> 0, "Changed orientation to 1")
 * and the capability-cache update fine; the last line ever
 * printed was "ROTATE: calling lv_display_set_resolution
 * (...)". Death is inside lv_display_set_resolution() —
 * i.e. inside update_resolution() — or in the refresh it
 * arms. Reading LVGL 9.3's update_resolution() shows only
 * bounded work (per-screen coords + LV_EVENT_SIZE_CHANGED
 * sends, a full object-tree walk marking layouts dirty,
 * invalidation state reset, LV_EVENT_RESOLUTION_CHANGED —
 * no listener for it exists in this tree) — but one piece
 * of it has never run at runtime on this device: the tree
 * walk descends into EVERY screen, including the production
 * status screen's widget tree, which no other runtime path
 * ever walks (refresh only descends into the ACTIVE screen,
 * and the status screen is covered here). A corrupt child
 * array there — the Deck UI churns objects at runtime —
 * would wedge or fault the walk while leaving normal
 * operation intact, matching the total silence. Stage 2
 * therefore now (a) walks the trees itself first, with
 * heartbeat logs that keep escaping (a cyclic tree shows
 * up as endless heartbeats, a healthy one as a total), and
 * (b) applies the resolution change by hand: resolution
 * fields, screen/layer coords, invalidation state —
 * exactly update_resolution()'s state changes minus the
 * event sends and the walk, neither of which the rebuilt
 * test screens need. If the wedge was inside
 * update_resolution(), this removes it; and the walk
 * result splits the remaining suspects either way.
 *
 * v11.3 (third hardware verdict): the walk completed over
 * 172 objects — the object trees are healthy, that theory
 * is dead — and the trail then stopped right after
 * "setting resolution fields directly". So the wedge is
 * in the hand-applied span itself (hor/ver stores, the
 * screens[] coords loop, layer coords, inv memzeros,
 * inv_p, lv_obj_invalidate(sys_layer)) — or just past it,
 * in the rebuild, with the "resolution now" line stranded
 * in the deferred log queue (no drain sleep followed it).
 * Static re-reading of the span: lv_area_set_* are pure
 * arithmetic; the private-struct layout CANNOT diverge
 * (compile_commands.json shows this TU and lv_display.c
 * built with identical -D sets and the same LV_CONF_PATH;
 * the struct's only config-sized member is
 * inv_areas[LV_INV_BUF_SIZE], and the config-conditional
 * members sit in the tail); Zephyr asserts are compiled
 * out (CONFIG_ASSERT unset), so LV_ASSERT_HANDLER is a
 * no-op. The one call in the span that enters LVGL's
 * event/visibility machinery — ext-draw-size style walks,
 * display lookup, visibility parent climb — is
 * lv_obj_invalidate(sys_layer), also the only machinery
 * call common to the v11 death (update_resolution calls
 * it too). v11.3 therefore (a) cross-checks this TU's
 * private-field reads against the library's own getters
 * before writing anything (read-only, prints the proof),
 * (b) replaces lv_obj_invalidate(sys_layer) with its raw
 * endpoint — one lv_inv_area() of the sys layer's full
 * coords, which is all update_resolution's invalidate
 * accomplishes (the sys layer is empty on this device),
 * and (c) brackets EVERY substep of the field application
 * and of the rebuild with a log + drain sleep, closing
 * the stranded-log hole as well.
 *
 * v11.4 (fourth hardware verdict): v11.3's trail ran clean
 * through the whole field application (fs1-fs5 done, the
 * layout check all matched, "resolution now" printed) and
 * then stopped right after "ROTATE: rb build pattern2" —
 * "rb load" never printed and touch stayed dead. The span
 * between those two log lines is exactly
 * build_pattern2_screen(): the field application and
 * lv_scr_load are both exonerated (lv_screen_load's
 * internal only swaps act_scr, sends screen events and
 * invalidates — no synchronous refresh exists in it to
 * wedge in, and the load step was never even reached).
 * What the rebuild did that no earlier code had: create
 * three full screens (~90 objects; the pattern2 page
 * alone ~59) while the three old screens and the
 * production status tree still held the 49,152-byte LVGL
 * sys_heap (CONFIG_LV_Z_MEM_POOL_SIZE /
 * _MEM_POOL_SYS_HEAP). lv_obj_create() returns NULL on
 * heap exhaustion (lv_obj_class_create_obj, LVGL 9.3) and
 * every builder in this file dereferences the result
 * unchecked; on this MCU that reads flash garbage or
 * faults inside the display-queue thread, which starves
 * the lowest-priority deferred logger — the observed
 * total silence after the last drained line. v11.4
 * inverts the rebuild order: load a fresh blank screen,
 * delete the three old screens, THEN build, so the
 * transient heap peak is one empty screen. An
 * lv_mem_monitor() line (used % plus the heap's
 * high-water mark, the two fields Zephyr's port fills)
 * brackets every rebuild step, so if the heap is ever
 * again the wall, the numbers say so instead of silence.
 *
 * v11.5 (fifth hardware verdict, adjudicated): the
 * photos from the v11.4 run — stack reading reversed in
 * landscape, black bands, touch disagreeing with the
 * drawn buttons — traced to the forked driver, not to
 * this file. The fork's MADCTL table (inherited from
 * Prospector) assigns the ST7789V standard rotation
 * bytes one step off from the Zephyr convention the
 * pointer glue implements: its ROTATED_90 wrote
 * MY|MV (0xA0, the standard 270) and its ROTATED_270
 * wrote MX|MV (0x60, the standard 90 — the byte the
 * patternls build proves on this panel), so at 90/270
 * the drawn image sat 180 degrees in-frame away from
 * the logical frame LVGL and the touch transform used.
 * The fork now writes the standard table (see
 * zmk-feature-esb-hid's display_st7789v.c). This file's
 * tap-screen geometry was audited against the photos
 * and is NOT the cause: in the logical frame the column
 * stacks TAP/PATTERN/ROTATE top-down and spans 88-89%
 * of the height at both resolutions, which cannot
 * produce a reversed stack or a band. (The "180" photo
 * state — portrait content covering ~75% of the panel
 * with a band — matches a portrait logical frame being
 * scanned under an MV landscape MADCTL, 240 of 320
 * GRAM rows: an off-by-one state, not the 180 chain,
 * whose MADCTL, margins and glue formula all check
 * out.) One genuine app-side defect did come out of
 * the audit, on pattern page 1: its content metrics
 * switched on the compile-time LANDSCAPE_PATTERN
 * config instead of the live resolution, so in this
 * build's runtime landscape the SHARP 28 sample
 * (y=235, ~28 px tall) ran past the 240-row bottom
 * edge; the compact metrics are now chosen from the
 * live height, which also leaves the landscape-pattern
 * variant (boot height 240) with the values it always
 * had. Pattern page 2 audited clean (BACK is
 * resolution-relative and the cursor-stacked sections
 * end by y=200 at both heights).
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
/* v11.2: stage 2 writes the display's resolution fields
 * directly instead of calling lv_display_set_resolution()
 * (see the header note), which needs the private struct.
 * Zephyr's LVGL port puts the LVGL src/ tree on the include
 * path, so this resolves. Prototype only — do not copy this
 * into production code. */
#include "display/lv_display_private.h"
#include "core/lv_obj_private.h"
/* v11.3: lv_inv_area() — the raw endpoint of the
 * lv_obj_invalidate() chain (see the frame step). */
#include "core/lv_refr_private.h"
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

/* The switch runs in two stages (v11.1, see the stage handlers
 * below): the panel step on the system work queue, the LVGL
 * reframe step on the display queue. pend_* carries the target
 * geometry from the first stage to the second. */
static enum display_orientation pend_zo;
static int32_t pend_w;
static int32_t pend_h;

static void rotate_panel_work_handler(struct k_work *work);
static K_WORK_DEFINE(rotate_panel_work, rotate_panel_work_handler);
static void rotate_frame_work_handler(struct k_work *work);
static K_WORK_DEFINE(rotate_frame_work, rotate_frame_work_handler);

static void rotate_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: rotate");

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    LOG_INF("ROTATE: click, current rot_idx=%u (%d deg), queueing switch", rot_idx,
            rot_idx * 90);

    /* Only queue the switch: the screens cannot be rebuilt from
     * inside this event (the button itself is on one of them).
     * The panel step goes to the system work queue, NOT the
     * display queue: the display queue is serial and carries
     * every LVGL tick, so a step that never returned there
     * would freeze input and refresh together — and with
     * deferred logging on the lowest-priority thread, its
     * buffered lines would never flush either (the v11
     * symptom: one ROTATE tap, then total silence). */
    k_work_submit(&rotate_panel_work);
    LOG_INF("TOUCHTEST rotate queued");
    LOG_INF("ROTATE: panel step submitted to system queue");
}

static void tap_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: tap");

    if (lv_event_get_code(e) != LV_EVENT_PRESSED) {
        return;
    }

    tap_count++;
    lv_label_set_text_fmt(tap_label, "TAP TEST: %u", tap_count);
    LOG_INF("TOUCHTEST tap #%u", tap_count);
}

static void pattern_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: pattern");

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 1;
    lv_scr_load(pattern_scr);
    LOG_INF("TOUCHTEST pattern screen loaded");
}

static void back_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: back");

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 0;
    lv_scr_load(tap_scr);
    LOG_INF("TOUCHTEST tap screen loaded");
}

static void next_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: next");

    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    cur_scr = 2;
    lv_scr_load(pattern2_scr);
    LOG_INF("TOUCHTEST pattern 2 screen loaded");
}

static void back2_btn_event_cb(lv_event_t *e) {
    LOG_INF("BTN: back2");

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

    /* Pure color bars, full width. In the 240-row orientations
     * the vertical rhythm (bars and text lines) is compacted so
     * that everything still fits. The choice follows the LIVE
     * height (v11.5): it used to switch on the compile-time
     * LANDSCAPE_PATTERN config, which left the runtime-rotation
     * build laying a landscape screen out with portrait metrics
     * (the SHARP 28 sample ran past the bottom edge). The
     * landscape-pattern variant boots at 240 rows, so it picks
     * the same compact values it always had. */
    static const uint32_t bar_colors[] = {0xFF0000, 0x00FF00, 0x0000FF,
                                          0xFFFFFF, 0x808080, 0x000000};
    const bool compact = (H <= 240);
    const int32_t bar_y0 = compact ? 110 : 112;
    const int32_t bar_step = compact ? 8 : 10;
    const int32_t bar_h = compact ? 7 : 9;

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
    static const int32_t advances_tall[] = {15, 19, 25, 31};
    static const int32_t advances_compact[] = {13, 15, 21, 29};
    const int32_t *advances = compact ? advances_compact : advances_tall;
    int32_t ty = compact ? 160 : 176;

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
    plain_rect(pattern2_scr, 6, y, W - 12, 22, 0x808080);
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

/* Live rotation switch (v11), staged in v11.1 after the first
 * hardware run wedged the Deck: a ROTATE tap killed all touch,
 * the screen froze as-is, and not one of the instrumented
 * ROTATE:/BTN: lines ever reached the log. The v11 sequence ran
 * entirely as one work item on the display queue — the serial
 * queue that also carries every LVGL tick (input + refresh) —
 * so anything in it that never returns (spin or fault) takes
 * the whole UI down with it, and because logging is deferred
 * to the lowest-priority thread, the buffered lines die
 * unsent. The prime suspect inside is the first-ever runtime
 * call of the forked driver's display_set_orientation() (the
 * same transmit path otherwise only ever runs from SYS_INIT
 * at boot), with the set_resolution + screen delete/rebuild
 * right behind it.
 *
 * v11.1 therefore splits the switch:
 *
 *  Stage 1 (system work queue): the panel step —
 *  display_set_orientation() only. It runs off the LVGL
 *  thread, so if the driver call is what wedges, LVGL and
 *  touch stay alive. Between the stages LVGL keeps flushing
 *  in the old frame against the rotated panel for a few
 *  ticks — a transient skew on a bring-up build, corrected
 *  by the full redraw in stage 2.
 *
 *  Stage 2 (display queue): the LVGL reframe — pointer glue
 *  capability cache, the resolution change, rebuild of the
 *  three test screens at the new resolution, reload, delete
 *  of the old screens. Pure LVGL work, which must stay on
 *  the display queue. (v11.2: the resolution change is
 *  applied by hand, not via lv_display_set_resolution() —
 *  see the header note.)
 *
 * Each stage logs, then sleeps ROTATE_LOG_DRAIN_MS before its
 * risky call, so the log thread gets the CPU and the trail is
 * physically out: the last ROTATE: line is the step that died.
 */
#define ROTATE_LOG_DRAIN_MS 200

static void rotate_panel_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    LOG_INF("ROTATE: panel step entry, rot_idx=%u (%d deg)", rot_idx, rot_idx * 90);
    k_msleep(ROTATE_LOG_DRAIN_MS);

    const uint8_t next_idx = (rot_idx + 1) & 0x3;
    const enum display_orientation zo = rot_cycle[next_idx];
    const int32_t w = (next_idx & 1) ? 280 : 240;
    const int32_t h = (next_idx & 1) ? 240 : 280;

    LOG_INF("ROTATE: target orientation=%d (%d deg), logical %dx%d", (int)zo, next_idx * 90,
            (int)w, (int)h);

    if (!device_is_ready(panel)) {
        LOG_ERR("TOUCHTEST rotate: panel not ready");
        LOG_ERR("ROTATE: panel not ready, aborting (nothing applied)");
        return;
    }

    LOG_INF("ROTATE: calling display_set_orientation(%d)", (int)zo);
    k_msleep(ROTATE_LOG_DRAIN_MS);
    int ret = display_set_orientation(panel, zo);
    LOG_INF("ROTATE: display_set_orientation -> %d", ret);

    if (ret != 0) {
        LOG_ERR("TOUCHTEST rotate: set_orientation -> %d", ret);
        LOG_ERR("ROTATE: set_orientation failed (%d), cap/resolution NOT updated", ret);
        return;
    }

    /* Commit only now: until the panel has actually switched,
     * rot_idx keeps describing the geometry LVGL still uses. */
    rot_idx = next_idx;
    pend_zo = zo;
    pend_w = w;
    pend_h = h;

    k_work_submit_to_queue(zmk_display_work_q(), &rotate_frame_work);
    LOG_INF("ROTATE: frame step submitted to display queue");
}

/* Tree-walk diagnostic (v11.2): counts every object in every
 * screen's tree. The heartbeat fires every 256 objects and
 * then sleeps briefly so the lowest-priority log thread can
 * physically flush it even mid-walk: if the walk never
 * returns, a healthy tree is excluded and the heartbeat
 * count at the stall (or its endless growth, for a cyclic
 * child array) is the evidence. */
static uint32_t rot_walk_count;

static lv_obj_tree_walk_res_t rot_walk_cb(lv_obj_t *obj, void *user_data) {
    ARG_UNUSED(obj);
    ARG_UNUSED(user_data);

    rot_walk_count++;
    if ((rot_walk_count % 256) == 0) {
        LOG_INF("ROTATE: walk heartbeat, %u objects", rot_walk_count);
        k_msleep(20);
    }
    return LV_OBJ_TREE_WALK_NEXT;
}

/* Heap monitor (v11.4): the LVGL heap is a 49,152-byte
 * sys_heap. Zephyr's lv_mem_monitor_core() fills only
 * used_pct and max_used (the heap's high-water mark);
 * max_used can only grow, so the rb-step trail records the
 * rebuild's true peak allocation. */
static void rot_log_mem(const char *tag) {
    lv_mem_monitor_t mon;

    lv_mem_monitor(&mon);
    LOG_INF("ROTATE: mem %s: used %u%%, max_used %u of 49152", tag, mon.used_pct,
            (unsigned)mon.max_used);
}

static void rotate_frame_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    const enum display_orientation zo = pend_zo;
    const int32_t w = pend_w;
    const int32_t h = pend_h;

    LOG_INF("ROTATE: frame step entry, rot_idx=%u (%d deg), logical %dx%d", rot_idx,
            rot_idx * 90, (int)w, (int)h);
    k_msleep(ROTATE_LOG_DRAIN_MS);

    lv_display_t *disp = lv_display_get_default();
    struct lvgl_disp_data *dd = lv_display_get_user_data(disp);

    if (dd != NULL) {
        LOG_INF("ROTATE: cap before: orientation=%d, %dx%d", (int)dd->cap.current_orientation,
                (int)dd->cap.x_resolution, (int)dd->cap.y_resolution);
        dd->cap.current_orientation = zo;
        dd->cap.x_resolution = w;
        dd->cap.y_resolution = h;
        LOG_INF("ROTATE: cap after: orientation=%d, %dx%d", (int)dd->cap.current_orientation,
                (int)dd->cap.x_resolution, (int)dd->cap.y_resolution);
    } else {
        LOG_ERR("ROTATE: lvgl_disp_data is NULL, cap NOT updated (panel rotated, glue cache "
                "stale)");
    }

    /* v11.2: lv_display_set_resolution() is bypassed (see the
     * header note). First walk every object tree ourselves —
     * the same descent update_resolution() would do — with
     * heartbeats, then apply its state changes by hand:
     * resolution fields, screen/layer coords, invalidation
     * state. Dropped on purpose: the LV_EVENT_SIZE_CHANGED
     * sends (the three test screens are rebuilt right below,
     * and the covered status screen re-lays itself out on its
     * next ZMK-driven update), the layout-dirty tree walk
     * (same reason), and LV_EVENT_RESOLUTION_CHANGED (no
     * listener exists in this tree). */
    LOG_INF("ROTATE: self-walk of object trees first");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    rot_walk_count = 0;
    lv_obj_tree_walk(NULL, rot_walk_cb, NULL);
    LOG_INF("ROTATE: tree walk complete, %u objects", rot_walk_count);
    k_msleep(ROTATE_LOG_DRAIN_MS);

    /* v11.3 layout cross-check (read-only, before any
     * write): this TU's private-field reads against the
     * LVGL library's own getters. Build evidence says the
     * layout cannot diverge (identical LV_CONF_PATH and
     * -D sets for this TU and lv_display.c); this prints
     * the runtime proof either way. */
    {
        lv_obj_t *lib_act = lv_display_get_screen_active(disp);
        bool act_listed = false;

        for (uint32_t i = 0; i < disp->screen_cnt; i++) {
            if (disp->screens[i] == lib_act) {
                act_listed = true;
            }
        }
        LOG_INF("ROTATE: layout check: hor field=%d getter=%d, sys %p/%p, top %p/%p, "
                "bottom %p/%p, act %p/%p listed=%d, screen_cnt=%u",
                (int)disp->hor_res, (int)lv_display_get_horizontal_resolution(disp),
                (void *)disp->sys_layer, (void *)lv_display_get_layer_sys(disp),
                (void *)disp->top_layer, (void *)lv_display_get_layer_top(disp),
                (void *)disp->bottom_layer, (void *)lv_display_get_layer_bottom(disp),
                (void *)disp->act_scr, (void *)lib_act, (int)act_listed,
                (unsigned)disp->screen_cnt);
        k_msleep(ROTATE_LOG_DRAIN_MS);
    }

    /* v11.3: every substep bracketed with a log + drain
     * sleep. fs5 replaces lv_obj_invalidate(sys_layer) —
     * the span's only call into LVGL's event/visibility
     * machinery, and the only machinery call common to
     * the v11 and v11.2 deaths — with its raw endpoint:
     * a single lv_inv_area() of the sys layer's full
     * (new) coords, which is all update_resolution()'s
     * invalidate accomplishes. The sys layer is empty on
     * this device, so nothing visual rides on it. */
    LOG_INF("ROTATE: setting resolution fields directly to %dx%d", (int)w, (int)h);
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: fs1 hor/ver write");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    disp->hor_res = w;
    disp->ver_res = h;
    LOG_INF("ROTATE: fs1 done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: fs2 screens loop, cnt=%u", (unsigned)disp->screen_cnt);
    k_msleep(ROTATE_LOG_DRAIN_MS);
    for (uint32_t i = 0; i < disp->screen_cnt; i++) {
        lv_area_set_width(&disp->screens[i]->coords, w);
        lv_area_set_height(&disp->screens[i]->coords, h);
    }
    LOG_INF("ROTATE: fs2 done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: fs3 layer coords");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    lv_area_set_width(&disp->top_layer->coords, w);
    lv_area_set_height(&disp->top_layer->coords, h);
    lv_area_set_width(&disp->sys_layer->coords, w);
    lv_area_set_height(&disp->sys_layer->coords, h);
    lv_area_set_width(&disp->bottom_layer->coords, w);
    lv_area_set_height(&disp->bottom_layer->coords, h);
    LOG_INF("ROTATE: fs3 done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: fs4 inv reset");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    lv_memzero(disp->inv_areas, sizeof(disp->inv_areas));
    lv_memzero(disp->inv_area_joined, sizeof(disp->inv_area_joined));
    disp->inv_p = 0;
    LOG_INF("ROTATE: fs4 done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: fs5 sys-layer inv_area (raw, replaces lv_obj_invalidate)");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    {
        lv_area_t sys_area;

        lv_area_copy(&sys_area, &disp->sys_layer->coords);
        lv_inv_area(disp, &sys_area);
    }
    LOG_INF("ROTATE: fs5 done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: resolution now %dx%d",
            (int)lv_display_get_horizontal_resolution(disp),
            (int)lv_display_get_vertical_resolution(disp));
    k_msleep(ROTATE_LOG_DRAIN_MS);

    lv_obj_t *old_tap = tap_scr;
    lv_obj_t *old_pattern = pattern_scr;
    lv_obj_t *old_pattern2 = pattern2_scr;

    /* v11.4: free before build (see the header note). A blank
     * screen takes over as the active screen, the three old
     * screens are deleted, and only then are the new screens
     * built — the rebuild's transient heap peak is one empty
     * screen instead of three full ones. */
    rot_log_mem("rb entry");
    LOG_INF("ROTATE: rb load blank");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    lv_obj_t *blank_scr = lv_obj_create(NULL);

    if (blank_scr == NULL) {
        LOG_ERR("ROTATE: blank screen allocation failed - LVGL heap already "
                "exhausted before the rebuild; aborting the switch");
        rot_log_mem("rb abort");
        LOG_INF("ROTATE: frame step exit");
        k_msleep(ROTATE_LOG_DRAIN_MS);
        return;
    }
    lv_obj_set_style_bg_color(blank_scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(blank_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(blank_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_scr_load(blank_scr);
    LOG_INF("ROTATE: rb load blank done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb delete olds");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    if (old_tap != NULL) {
        lv_obj_delete(old_tap);
    }
    if (old_pattern != NULL) {
        lv_obj_delete(old_pattern);
    }
    if (old_pattern2 != NULL) {
        lv_obj_delete(old_pattern2);
    }
    LOG_INF("ROTATE: rb delete olds done");
    rot_log_mem("after delete olds");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb build tap");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    build_tap_screen();
    LOG_INF("ROTATE: rb build tap done");
    rot_log_mem("after build tap");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb build pattern");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    build_pattern_screen();
    LOG_INF("ROTATE: rb build pattern done");
    rot_log_mem("after build pattern");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb build pattern2");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    build_pattern2_screen();
    LOG_INF("ROTATE: rb build pattern2 done");
    rot_log_mem("after build pattern2");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb load");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    lv_scr_load(cur_scr == 2 ? pattern2_scr : (cur_scr == 1 ? pattern_scr : tap_scr));
    LOG_INF("ROTATE: rb load done");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: rb delete blank");
    k_msleep(ROTATE_LOG_DRAIN_MS);
    lv_obj_delete(blank_scr);
    LOG_INF("ROTATE: rb delete blank done");
    rot_log_mem("rb exit");
    k_msleep(ROTATE_LOG_DRAIN_MS);

    LOG_INF("ROTATE: screens rebuilt + reloaded (cur_scr=%d), sequence complete", cur_scr);
    LOG_INF("TOUCHTEST rotate -> %d deg (%dx%d)", rot_idx * 90, (int)w, (int)h);
    LOG_INF("ROTATE: frame step exit");
    k_msleep(ROTATE_LOG_DRAIN_MS);
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
