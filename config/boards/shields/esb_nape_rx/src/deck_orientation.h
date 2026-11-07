/*
 * Deck display orientation (live rotation, phase 2).
 *
 * The orientation index (0..3, quarter turns from NORMAL) is
 * persisted in settings under "deck/orientation" and applied at
 * boot from orientation.c's SYS_INIT, before LVGL init, so the
 * whole stack (capabilities, draw buffers, pointer transform)
 * is built for the saved orientation. At runtime the UI cycles
 * it live with the sequence proven by touchtest v11.4: panel
 * step on the system work queue, LVGL reframe on the display
 * queue, screens deleted before they are rebuilt so the LVGL
 * heap never holds two generations at once.
 *
 * status_screen.c drives the switch through these two calls;
 * everything else (settings, boot apply, work items) lives in
 * orientation.c. Compiled only when CONFIG_ESB_NAPE_RX_ROTATION
 * is on (the disptest Deck build); the plain receiver build has
 * no rotation and no settings subtree.
 */
#ifndef DECK_ORIENTATION_H
#define DECK_ORIENTATION_H

#include <stdint.h>

/* Current orientation index, 0..3 (0 = NORMAL/portrait as
 * before, 1..3 = quarter turns). Tracks the geometry LVGL is
 * actually using: during a live switch it advances only after
 * the panel step has succeeded. */
uint8_t deck_orientation_get(void);

/* Cycle to the next orientation: 0 -> 1 -> 2 -> 3 -> 0.
 * The switch is asynchronous (two staged work items) and is
 * ignored while a previous switch is still in flight. */
void deck_orientation_cycle_next(void);

#endif /* DECK_ORIENTATION_H */
