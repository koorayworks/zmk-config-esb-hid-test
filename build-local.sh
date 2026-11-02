#!/usr/bin/env bash
# Build TX and RX test firmware for HID over ESB (step 1).
#
# Setup (once), from the parent directory of this repository clone:
#   west init -l zmk-config-esb-hid-test/config
#   west update
#   west zephyr-export
# Then run this script from the repository root.
set -euo pipefail
cd "$(dirname "$0")"

BOARD="${BOARD:-xiao_ble/nrf52840}"
ZMK_APP="${ZMK_APP:-$PWD/zmk/app}"

# Test targets disabled 2026-10-29 (uncomment to re-enable).
# west build -p -s "$ZMK_APP" -b "$BOARD" -d build/tx -- \
#   -DSHIELD=esb_test_tx -DZMK_CONFIG="$PWD/config"
#
# west build -p -s "$ZMK_APP" -b "$BOARD" -d build/rx -- \
#   -DSHIELD=esb_test_rx -DZMK_CONFIG="$PWD/config"

# Nickey TX: real Nickey left-half PCB as a standalone ESB TX
# (re-enabled 2026-10-31 for the Step 2 multi-device work).
# Uses the xiao_ble//zmk board extension like TentAlice/Nape: the
# plain xiao_ble/nrf52840 configuration has no FLASH partition for
# settings, so the module's SETTINGS backend fell back to NONE and a
# pairing-assigned slot would not persist across reboots (Step 2
# follow-up, verified: //zmk yields CONFIG_SETTINGS_NVS=y).
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nickey-tx -- \
  -DSHIELD=esb_nickey_tx -DZMK_CONFIG="$PWD/config"

# koZakura TX: real koZakura48 left-half PCB as a standalone ESB TX
# (added 2026-10-31 for the Step 2 multi-device work).
# Same xiao_ble//zmk rationale as Nickey above (NVS persistence).
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/kozakura-tx -- \
  -DSHIELD=esb_kozakura_tx -DZMK_CONFIG="$PWD/config"

# TentAlice uses the xiao_ble//zmk board extension (like the original
# zmk-config-tentalice build.yaml): it wires up UF2 boot-mode via
# GPREGRET retention, without which &bootloader only resets.
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/tentalice-tx -- \
  -DSHIELD=esb_tentalice_tx -DZMK_CONFIG="$PWD/config"

# Nape receiver: ESB RX + ST7789V display + CST816S touch + PAW3222.
# Needs the xiao_ble//zmk variant (battery readout) like TentAlice.
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nape-rx -- \
  -DSHIELD=esb_nape_rx -DZMK_CONFIG="$PWD/config"

# Nape Deck production build: esb_nape_rx + esb_nape_disptest layered
# on top. The unit wires the display like the Prospector adapter; the
# trackball sensor is disabled (its pins collide with the display)
# and the CST816T touch panel is integrated on i2c1 (Step 2).
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nape-disptest -- \
  "-DSHIELD=esb_nape_rx;esb_nape_disptest" -DZMK_CONFIG="$PWD/config"

# Nape touch test: disptest + esb_nape_touchtest layered on top.
# Touch itself now comes from the disptest layer; this shield only
# adds an I2C bus scan + on-screen tap counter / test pattern for
# bring-up.
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nape-touchtest -- \
  "-DSHIELD=esb_nape_rx;esb_nape_disptest;esb_nape_touchtest" -DZMK_CONFIG="$PWD/config"

mkdir -p firmware
# cp build/tx/zephyr/zmk.uf2 firmware/esb_tx.uf2
# cp build/rx/zephyr/zmk.uf2 firmware/esb_rx.uf2
cp build/nickey-tx/zephyr/zmk.uf2 firmware/esb_nickey_tx.uf2
cp build/kozakura-tx/zephyr/zmk.uf2 firmware/esb_kozakura_tx.uf2
cp build/tentalice-tx/zephyr/zmk.uf2 firmware/esb_tentalice_tx.uf2
cp build/nape-rx/zephyr/zmk.uf2 firmware/esb_nape_rx.uf2
cp build/nape-disptest/zephyr/zmk.uf2 firmware/esb_nape_disptest.uf2
cp build/nape-touchtest/zephyr/zmk.uf2 firmware/esb_nape_touchtest.uf2

# echo "TX: firmware/esb_tx.uf2"
# echo "RX: firmware/esb_rx.uf2"
echo "Nickey TX: firmware/esb_nickey_tx.uf2"
echo "koZakura TX: firmware/esb_kozakura_tx.uf2"
echo "Tentalice TX: firmware/esb_tentalice_tx.uf2"
echo "Nape RX: firmware/esb_nape_rx.uf2"
echo "Nape display test: firmware/esb_nape_disptest.uf2"
echo "Nape touch test: firmware/esb_nape_touchtest.uf2"
