if(CONFIG_SHIELD_ESB_NAPE_RX)
  zephyr_library()
  zephyr_library_include_directories(${ZEPHYR_LVGL_MODULE_DIR})
  zephyr_library_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl/)
  zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)
  zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
  zephyr_library_sources(src/status_screen.c)
  zephyr_library_sources(src/brightness.c)
  # Custom-converted Montserrat SemiBold fonts (src/fonts/), only
  # for the Deck builds that opt in via ESB_NAPE_RX_CUSTOM_FONT;
  # the plain receiver build keeps the stock LVGL Montserrat.
  if(CONFIG_ESB_NAPE_RX_CUSTOM_FONT)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb_12.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb_14.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb_16.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb_20.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb_28.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_b_20.c)
    # bpp 1 SemiBold (pattern page 2 samples only, never routed
    # into the UI; unreferenced in the disptest build and
    # garbage-collected there).
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb1_14.c)
    zephyr_library_sources(src/fonts/lv_font_montserrat_sb1_20.c)
    # bpp 8 conversion of the same SemiBold set; only linked
    # when the BPP8 switch selects it (it depends on the
    # custom-font switch, so this nests safely inside).
    if(CONFIG_ESB_NAPE_RX_FONT_BPP8)
      zephyr_library_sources(src/fonts/lv_font_montserrat_sb8_12.c)
      zephyr_library_sources(src/fonts/lv_font_montserrat_sb8_14.c)
      zephyr_library_sources(src/fonts/lv_font_montserrat_sb8_16.c)
      zephyr_library_sources(src/fonts/lv_font_montserrat_sb8_20.c)
      zephyr_library_sources(src/fonts/lv_font_montserrat_sb8_28.c)
    endif()
  endif()
endif()
