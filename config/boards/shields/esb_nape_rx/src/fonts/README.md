# Custom fonts for the Nape Deck UI

Generated LVGL font files used by `../status_screen.c` when
`CONFIG_ESB_NAPE_RX_CUSTOM_FONT=y` (set by the disptest and
touchtest shield confs; the plain `esb_nape_rx` build keeps the
stock LVGL Montserrat).

## Source

Montserrat, designed by Julieta Ula — the Montserrat Project:

- SemiBold: `Montserrat-SemiBold.ttf`
  https://github.com/JulietaUla/Montserrat/raw/master/fonts/ttf/Montserrat-SemiBold.ttf
- Bold: `Montserrat-Bold.ttf`
  https://github.com/JulietaUla/Montserrat/raw/master/fonts/ttf/Montserrat-Bold.ttf

Retrieved 2026-11-04 from the `master` branch of
https://github.com/JulietaUla/Montserrat

## License

SIL Open Font License, Version 1.1 — see `OFL.txt` in this
directory (copied verbatim from the source repository).
Copyright 2024 The Montserrat.Git Project Authors.

## Conversion

Tool: `lv_font_conv` (npm, https://github.com/lvgl/lv_font_conv),
run 2026-11-04. Same parameters as the stock LVGL Montserrat
(bpp 4, no compression, same character set: 0x20-0x7F plus
0xB0 DEGREE SIGN and 0x2022 BULLET — the only non-ASCII
codepoints the stock fonts carry; status_screen.c uses no
non-ASCII characters in its strings).

```
lv_font_conv --font Montserrat-SemiBold.ttf --size 16 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_sb_16 -o lv_font_montserrat_sb_16.c

lv_font_conv --font Montserrat-SemiBold.ttf --size 20 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_sb_20 -o lv_font_montserrat_sb_20.c

lv_font_conv --font Montserrat-SemiBold.ttf --size 28 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_sb_28 -o lv_font_montserrat_sb_28.c

lv_font_conv --font Montserrat-Bold.ttf --size 20 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_b_20 -o lv_font_montserrat_b_20.c
lv_font_conv --font Montserrat-SemiBold.ttf --size 12 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_sb_12 -o lv_font_montserrat_sb_12.c

lv_font_conv --font Montserrat-SemiBold.ttf --size 14 --bpp 4 \
  --format lvgl --no-compress --no-prefilter --force-fast-kern-format \
  --lv-include lvgl.h \
  -r 0x20-0x7F -r 0xB0 -r 0x2022 \
  --lv-font-name lv_font_montserrat_sb_14 -o lv_font_montserrat_sb_14.c
```

SemiBold 12 and 14 are bundled as judging samples for the
touchtest pattern page 2 only (does the heavier face make small
sizes usable again — the expectation is 14 as the floor). The
production status UI uses 16/20/28 until that is judged on
hardware.
