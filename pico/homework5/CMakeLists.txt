add_executable(homework5)

target_sources(homework5 PRIVATE homework5.c)

target_link_libraries(homework5 pico_stdlib hardware_i2c)

pico_add_extra_outputs(homework5)

pico_enable_stdio_usb(homework5 1) 
pico_enable_stdio_uart(homework5 0)