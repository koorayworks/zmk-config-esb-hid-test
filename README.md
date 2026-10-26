# ESB HID Step 1 テスト用 zmk-config

HID over ESB の実機テスト用、最小構成の zmk-config。
TX（キーボード側）と RX（レシーバー側）の2つのシールドを持つ。

| シールド | 役割 | 構成 |
|---|---|---|
| `esb_test_tx` | キーボード側 | XIAO nRF52840、GPIO直結4キー、USB/BLE 無効、ESB 送信 |
| `esb_test_rx` | レシーバー側 | XIAO nRF52840、ダミー1キー、USB HID 出力、ESB 受信 |

## キー配線（両シールド共通の考え方）

XIAO のピンを GND に落とすと押下になる（内部プルアップ、active low）。
ブレッドボードやジャンパで触れるだけでもテストできる。

| キー | XIAOピン | nRFピン |
|---|---|---|
| 1 | D0 | P0.02 |
| 2 | D1 | P0.03 |
| 3 | D4 | P0.04 |
| 4 | D5 | P0.05 |

TX のキーマップ: キー1=A、キー2=B、キー3=音量+、キー4=音量−。
マウス（ポインティング）の確認は、TX にトラックボールを繋ぐ段階で別途行う
（送信経路自体はモジュールに実装済み）。

RX のキー1はダミー（レシーバーにマトリクスが要るための最小構成）。

## ビルド

前提: west ワークスペース内に、フォーク版 `zmk`（ブランチ `esb-hid`）と
モジュール `zmk-feature-esb-hid` がこのフォルダの隣にチェックアウトされていること
（`~/workspace/esb-hid/` の並びがそのまま使える）。

    ./build-local.sh

成果物: `build/tx/zephyr/zmk.uf2` と `build/rx/zephyr/zmk.uf2`。
それぞれを XIAO に書き込み、RX 側を PC に USB 接続すれば、
TX のキーを押した入力が PC に届くはず。

## 注意

- TX/RX で ZMK の HID レポート設定（NKRO 等）を変えないこと。
  レポート形式が食い違うと受信側で正しく解釈できない。両側とも既定値で揃える。
- RX の USB 出力は report protocol 専用（boot protocol 非対応、Step 1 の割り切り）。
- ESB のアドレスは固定（モジュール内の Step 1 用定数）。ペアリングや暗号化は Step 2。

## セットアップ（GitHub起点）

このリポジトリを clone した親ディレクトリで:

    west init -l zmk-config-esb-hid-test/config
    west update
    west zephyr-export

`config/west.yml` がフォーク版 zmk（ブランチ `esb-hid`）と
モジュール `zmk-feature-esb-hid` を引いてくる。以後は `./build-local.sh` でビルド。

## 実機確認チェックリスト

用意するもの: XIAO nRF52840 を2台、ジャンパ線数本、PC。

1. TX 用 UF2 を1台目、RX 用 UF2 を2台目に書き込む。
2. RX を PC に USB 接続する（キーボードとして認識される）。
3. TX に電源を入れる（USB 給電でよい。HID は USB から出ない設定）。
4. TX の D0 と GND をジャンパ線で触れる → PC に `a` が入れば打鍵経路は成功。
5. D1 で `b`、D4/D5 で音量＋/− も確認する。
6. 触れている間だけ入力され、離すと止まること（押しっぱなしにならない）も見る。

### うまくいかないときの切り分け

- **何も起きない**: 以前に別の ZMK ファームを入れていた XIAO は、設定領域に
  旧ファームの出力先設定が残っていることがある。ZMK の settings reset 用
  UF2 を一度書いてから、TX/RX を書き直してみる。
- **RX が PC に認識されない**: ケーブルが充電専用でないか確認。
- **TX だけ電源を入れ直しても復帰するか**、**距離を離すとどうなるか**も
  ついでに見ておくと Step 2 の判断材料になる。
- 遅延の体感と、電池駆動での消費電力（測れる環境があれば）も Step 1 の
  ゴールなので、気づいたことをメモしてほしい。
