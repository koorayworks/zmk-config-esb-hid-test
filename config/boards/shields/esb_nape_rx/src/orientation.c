/*
 * Deck display orientation — live rotation, phase 2.
 *
 * Ports the rotation sequence hardware-proven by
 * esb_nape_touchtest v11.4-v11.6 (live cycle 0 -> 90 -> 180 ->
 * 270 -> 0, touch following) into the production Deck UI. The
 * differences from the test app are deliberate:
 *
 *  - No probe instrumentation: the ROTATE_LOG_DRAIN_MS sleeps
 *    and the tree-walk / layout-check / heap-monitor brackets
 *    of the bisect builds are gone, so a switch takes well
 *    under a second.
 *  - The rebuild target is ZMK's single status screen (the
 *    five pages and the tab bar live inside it), re-created by
 *    calling zmk_display_status_screen() again, instead of
 *    three test screens.
 *  - The orientation is persisted (settings "deck/orientation",
 *    u8 index 0..3) and applied at boot before LVGL init (see
 *    the SYS_INIT at the bottom), so a reboot restores it with
 *    the whole stack built for that orientation.
 *
 * Why the sequence has this exact shape (all hardware-verified
 * during the touchtest bisect; see touch_test.c's header notes
 * for the full trail):
 *
 *  - LVGL has no working software rotation on this stack:
 *    lv_refr never rotates pixels and the Zephyr glue's flush
 *    is a raw passthrough. The panel rotates instead, via the
 *    forked ST7789V driver's display_set_orientation()
 *    (MADCTL + margins, zmk-feature-esb-hid).
 *  - The panel step runs on the SYSTEM work queue, not the
 *    display queue. The display queue is serial and carries
 *    every LVGL tick (input + refresh); a step that never
 *    returned there would freeze input and refresh together,
 *    and deferred logging would never flush either (the v11
 *    symptom: one ROTATE tap, then total silence).
 *  - lv_display_set_resolution() is bypassed: the death point
 *    of v11.1 was inside it, and the state it maintains is
 *    applied by hand instead (resolution fields, screen/layer
 *    coords, invalidation state — minus the size-changed event
 *    sends and the layout-dirty tree walk, neither of which a
 *    full screen rebuild needs). Touch follows because the
 *    pointer glue transforms each event against the
 *    capabilities cached in the display's user data
 *    (struct lvgl_disp_data, <lvgl_display.h>), updated in
 *    the same step.
 *  - The screen is rebuilt free-before-build: load a blank
 *    screen, delete the old screen, THEN build the new one.
 *    The LVGL heap is 49,152 bytes; building the new tree
 *    while the old one (~172 objects) still held it exhausted
 *    the heap, lv_obj_create() returned NULL, and the
 *    unchecked dereference killed the display thread silently
 *    (the v11.3 death). The blank screen also gives the
 *    refresh a valid target while the rebuild runs.
 *
 * Settings: the handler is registered dynamically
 * (settings_register) so that the boot wheel below can
 * register it early and drive the load itself.
 * The settings subsystem is already on in the Deck build
 * (CONFIG_SETTINGS=y via the module's RX side), and saving
 * matches the module's own practice (settings_save_one).
 *
 * Boot apply ordering: the panel driver inits at POST_KERNEL,
 * LVGL at SYS_INIT(APPLICATION, 90), ZMK's display work later
 * still. Applying at SYS_INIT(APPLICATION, 50) lands between
 * the two. settings_subsys_init() is public and idempotent;
 * ZMK's main() calls it again plus the full settings_load()
 * afterwards, which is harmless (the value is already applied
 * and there are no other "deck" keys to load late).
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
#include <lvgl_display.h>
/* The frame step writes the display's resolution fields
 * directly instead of calling lv_display_set_resolution()
 * (see the header note), which needs the private struct.
 * Zephyr's LVGL port puts the LVGL src/ tree on the include
 * path (the esb_nape_rx shield CMakeLists mirrors the
 * touchtest one). */
#include "display/lv_display_private.h"
#include "core/lv_obj_private.h"
#include "core/lv_refr_private.h"

#include <zmk/display.h>
#include <zmk/display/status_screen.h>

#include "deck_orientation.h"

LOG_MODULE_REGISTER(deck_orientation, LOG_LEVEL_INF);

/* ---- State ---- */

static const struct device *const panel_dev = DEVICE_DT_GET(DT_NODELABEL(st7789v));

static const enum display_orientation orient_cycle[4] = {
    DISPLAY_ORIENTATION_NORMAL,
    DISPLAY_ORIENTATION_ROTATED_90,
    DISPLAY_ORIENTATION_ROTATED_180,
    DISPLAY_ORIENTATION_ROTATED_270,
};

/* The orientation LVGL is currently framed for. During a live
 * switch this advances only after the panel step succeeded,
 * so it always describes the live geometry. */
static uint8_t cur_orient_idx;

/* A switch in flight: the frame step consumes pend_*. */
static bool switch_busy;
static enum display_orientation pend_zo;
static int32_t pend_w;
static int32_t pend_h;

/* ---- Settings ("deck/orientation", u8 0..3) ---- */

static uint8_t saved_orient_idx;
static bool saved_orient_valid;

static int deck_orientation_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                                         void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "orientation", &next) && !next) {
        if (len != sizeof(saved_orient_idx)) {
            return -EINVAL;
        }
        int rc = read_cb(cb_arg, &saved_orient_idx, sizeof(saved_orient_idx));

        if (rc < 0) {
            return rc;
        }
        if (saved_orient_idx > 3) {
            saved_orient_idx = 0;
        }
        saved_orient_valid = true;
        return 0;
    }
    return -ENOENT;
}

static struct settings_handler deck_orientation_handler = {
    .name = "deck",
    .h_set = deck_orientation_settings_set,
};

/* ---- The live switch ---- */

static void rotate_panel_work_handler(struct k_work *work);
static K_WORK_DEFINE(rotate_panel_work, rotate_panel_work_handler);
static void rotate_frame_work_handler(struct k_work *work);
static K_WORK_DEFINE(rotate_frame_work, rotate_frame_work_handler);

/* Stage 1 (system work queue): the panel step —
 * display_set_orientation() only. Runs off the LVGL thread,
 * so a driver call that misbehaved could not take the UI
 * down with it (the v11 lesson). */
static void rotate_panel_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    const uint8_t next_idx = (cur_orient_idx + 1) & 0x3;
    const enum display_orientation zo = orient_cycle[next_idx];
    const int32_t w = (next_idx & 1) ? 280 : 240;
    const int32_t h = (next_idx & 1) ? 240 : 280;

    if (!device_is_ready(panel_dev)) {
        LOG_ERR("orientation: panel not ready, switch aborted");
        switch_busy = false;
        return;
    }

    int ret = display_set_orientation(panel_dev, zo);

    if (ret != 0) {
        LOG_ERR("orientation: set_orientation -> %d, switch aborted", ret);
        switch_busy = false;
        return;
    }

    /* Commit only now: until the panel has actually switched,
     * cur_orient_idx keeps describing the geometry LVGL still
     * uses. */
    cur_orient_idx = next_idx;
    pend_zo = zo;
    pend_w = w;
    pend_h = h;

    k_work_submit_to_queue(zmk_display_work_q(), &rotate_frame_work);
}

/* Stage 2 (display queue): the LVGL reframe — pointer glue
 * capability cache, the hand-applied resolution change, and
 * the screen rebuild (blank -> delete old -> build new ->
 * load -> delete blank). Pure LVGL work, which must stay on
 * the display queue. */
static void rotate_frame_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    const enum display_orientation zo = pend_zo;
    const int32_t w = pend_w;
    const int32_t h = pend_h;

    lv_display_t *disp = lv_display_get_default();
    struct lvgl_disp_data *dd = lv_display_get_user_data(disp);

    if (dd != NULL) {
        dd->cap.current_orientation = zo;
        dd->cap.x_resolution = w;
        dd->cap.y_resolution = h;
    } else {
        LOG_ERR("orientation: lvgl_disp_data is NULL, pointer cache stale");
    }

    /* Hand-applied resolution change (see the header note):
     * resolution fields, screen/layer coords, invalidation
     * state. The sys-layer invalidation goes through the raw
     * lv_inv_area() endpoint rather than lv_obj_invalidate():
     * that call was the one piece of event machinery inside
     * the wedging span in both the v11 and v11.2 deaths. */
    disp->hor_res = w;
    disp->ver_res = h;
    for (uint32_t i = 0; i < disp->screen_cnt; i++) {
        lv_area_set_width(&disp->screens[i]->coords, w);
        lv_area_set_height(&disp->screens[i]->coords, h);
    }
    lv_area_set_width(&disp->top_layer->coords, w);
    lv_area_set_height(&disp->top_layer->coords, h);
    lv_area_set_width(&disp->sys_layer->coords, w);
    lv_area_set_height(&disp->sys_layer->coords, h);
    lv_area_set_width(&disp->bottom_layer->coords, w);
    lv_area_set_height(&disp->bottom_layer->coords, h);
    lv_memzero(disp->inv_areas, sizeof(disp->inv_areas));
    lv_memzero(disp->inv_area_joined, sizeof(disp->inv_area_joined));
    disp->inv_p = 0;
    {
        lv_area_t sys_area;

        lv_area_copy(&sys_area, &disp->sys_layer->coords);
        lv_inv_area(disp, &sys_area);
    }

    /* Rebuild free-before-build (see the header note). The
     * old screen is the currently loaded one; the status
     * screen builder re-reads all of its state from the ESB
     * module, so nothing is lost by discarding the tree. */
    lv_obj_t *old_scr = lv_display_get_screen_active(disp);
    lv_obj_t *blank_scr = lv_obj_create(NULL);

    if (blank_scr == NULL) {
        LOG_ERR("orientation: blank screen allocation failed, switch aborted");
        switch_busy = false;
        return;
    }
    lv_obj_set_style_bg_color(blank_scr, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(blank_scr, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_clear_flag(blank_scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_scr_load(blank_scr);

    if (old_scr != NULL) {
        lv_obj_delete(old_scr);
    }

    lv_obj_t *new_scr = zmk_display_status_screen();

    if (new_scr == NULL) {
        LOG_ERR("orientation: status screen rebuild failed; blank screen left active");
        switch_busy = false;
        return;
    }

    lv_scr_load(new_scr);
    lv_obj_delete(blank_scr);

    LOG_INF("orientation: now %d deg (%dx%d)", cur_orient_idx * 90, (int)w, (int)h);

    /* Persist after the switch has fully landed, so a failed
     * switch never records a geometry the device is not in. */
    uint8_t to_save = cur_orient_idx;
    int err = settings_save_one("deck/orientation", &to_save, sizeof(to_save));

    if (err != 0) {
        LOG_WRN("orientation: settings save failed (%d)", err);
    }
    switch_busy = false;
}

/* ---- Public API (deck_orientation.h) ---- */

uint8_t deck_orientation_get(void) {
    return cur_orient_idx;
}

void deck_orientation_cycle_next(void) {
    if (switch_busy) {
        return;
    }
    switch_busy = true;
    k_work_submit(&rotate_panel_work);
}

/* ---- Boot: register the handler, load the saved value,
 *      apply it before LVGL init ---- */

static int deck_orientation_boot_apply(void) {
    int err = settings_subsys_init();

    if (err != 0) {
        LOG_WRN("orientation: settings_subsys_init -> %d", err);
    }
    err = settings_register(&deck_orientation_handler);
    if (err != 0) {
        LOG_WRN("orientation: settings_register -> %d", err);
    }
    err = settings_load_subtree("deck");
    if (err != 0) {
        LOG_WRN("orientation: settings_load_subtree -> %d", err);
    }

    if (!saved_orient_valid || saved_orient_idx == 0) {
        return 0;
    }
    if (!device_is_ready(panel_dev)) {
        LOG_ERR("orientation: panel not ready at boot apply");
        return 0;
    }
    int ret = display_set_orientation(panel_dev, orient_cycle[saved_orient_idx]);

    if (ret == 0) {
        cur_orient_idx = saved_orient_idx;
        LOG_INF("orientation: boot-applied %d deg", saved_orient_idx * 90);
    } else {
        LOG_ERR("orientation: boot set_orientation -> %d", ret);
    }
    return 0;
}

SYS_INIT(deck_orientation_boot_apply, APPLICATION, 50);
