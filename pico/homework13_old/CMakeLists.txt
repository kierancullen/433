add_executable(homework13)

target_sources(homework13 PRIVATE homework13.c MPU6050M.c)

target_link_libraries(homework13 pico_stdlib hardware_i2c)

pico_add_extra_outputs(homework13)

pico_enable_stdio_usb(homework13 1) 
pico_enable_stdio_uart(homework13 0)