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

BOARD="${BOARD:-seeeduino_xiao_ble}"
ZMK_APP="${ZMK_APP:-$PWD/../zmk/app}"

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/tx -- \
  -DSHIELD=esb_test_tx -DZMK_CONFIG="$PWD/config"

west build -p -s "$ZMK_APP" -b "$BOARD" -d build/rx -- \
  -DSHIELD=esb_test_rx -DZMK_CONFIG="$PWD/config"

echo "TX: build/tx/zephyr/zmk.uf2"
echo "RX: build/rx/zephyr/zmk.uf2"
