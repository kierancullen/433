add_executable(homework4)

target_sources(homework4 PRIVATE homework4.c)

target_link_libraries(homework4 pico_stdlib hardware_spi)

pico_add_extra_outputs(homework4)

pico_enable_stdio_usb(homework4 1) 
pico_enable_stdio_uart(homework4 0)