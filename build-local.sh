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

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/tentalice-tx -- \
  -DSHIELD=esb_tentalice_tx -DZMK_CONFIG="$PWD/config"

mkdir -p firmware
cp build/tx/zephyr/zmk.uf2 firmware/esb_tx.uf2
cp build/rx/zephyr/zmk.uf2 firmware/esb_rx.uf2
cp build/nickey-tx/zephyr/zmk.uf2 firmware/esb_nickey_tx.uf2
cp build/tentalice-tx/zephyr/zmk.uf2 firmware/esb_tentalice_tx.uf2

echo "TX: firmware/esb_tx.uf2"
echo "RX: firmware/esb_rx.uf2"
echo "Nickey TX: firmware/esb_nickey_tx.uf2"
echo "Tentalice TX: firmware/esb_tentalice_tx.uf2"
