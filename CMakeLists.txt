cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_1in54v2_epaper_lib_test)

pico_sdk_init()

add_subdirectory(lib)

add_executable(pico_1in54v2_epaper_lib_test
  test.c
)

# This will enable serial output via USB
pico_enable_stdio_usb(pico_1in54v2_epaper_lib_test 1)

pico_add_extra_outputs(pico_1in54v2_epaper_lib_test)

target_link_libraries(pico_1in54v2_epaper_lib_test epd_1in54_v2 epd_gui_paint epd_fonts)