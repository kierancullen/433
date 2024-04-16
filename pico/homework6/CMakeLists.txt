add_executable(homework6)

target_sources(homework6 PRIVATE homework6.c ssd1306.c font.c)

target_link_libraries(homework6 pico_stdlib hardware_i2c)

pico_add_extra_outputs(homework6)

pico_enable_stdio_usb(homework6 1) 
pico_enable_stdio_uart(homework6 0)