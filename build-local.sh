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

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/tx -- \
  -DSHIELD=esb_test_tx -DZMK_CONFIG="$PWD/config"

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/rx -- \
  -DSHIELD=esb_test_rx -DZMK_CONFIG="$PWD/config"

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/nickey-tx -- \
  -DSHIELD=esb_nickey_tx -DZMK_CONFIG="$PWD/config"

# TentAlice uses the xiao_ble//zmk board extension (like the original
# zmk-config-tentalice build.yaml): it wires up UF2 boot-mode via
# GPREGRET retention, without which &bootloader only resets.
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/tentalice-tx -- \
  -DSHIELD=esb_tentalice_tx -DZMK_CONFIG="$PWD/config"

# Nape receiver: ESB RX + ST7789V display + CST816S touch + PAW3222.
# Needs the xiao_ble//zmk variant (battery readout) like TentAlice.
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nape-rx -- \
  -DSHIELD=esb_nape_rx -DZMK_CONFIG="$PWD/config"

# Nape display test: esb_nape_rx + esb_nape_disptest layered on top.
# The test unit wires the display like the Prospector adapter and has
# no sensor/touch connected (sensor parked on dummy pins).
west build -p -s "$ZMK_APP" -b "xiao_ble//zmk" -d build/nape-disptest -- \
  "-DSHIELD=esb_nape_rx;esb_nape_disptest" -DZMK_CONFIG="$PWD/config"

mkdir -p firmware
cp build/tx/zephyr/zmk.uf2 firmware/esb_tx.uf2
cp build/rx/zephyr/zmk.uf2 firmware/esb_rx.uf2
cp build/nickey-tx/zephyr/zmk.uf2 firmware/esb_nickey_tx.uf2
cp build/tentalice-tx/zephyr/zmk.uf2 firmware/esb_tentalice_tx.uf2
cp build/nape-rx/zephyr/zmk.uf2 firmware/esb_nape_rx.uf2
cp build/nape-disptest/zephyr/zmk.uf2 firmware/esb_nape_disptest.uf2

echo "TX: firmware/esb_tx.uf2"
echo "RX: firmware/esb_rx.uf2"
echo "Nickey TX: firmware/esb_nickey_tx.uf2"
echo "Tentalice TX: firmware/esb_tentalice_tx.uf2"
echo "Nape RX: firmware/esb_nape_rx.uf2"
echo "Nape display test: firmware/esb_nape_disptest.uf2"
