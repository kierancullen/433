add_executable(homework12)

target_sources(homework12 PRIVATE homework12.c)

target_link_libraries(homework12 pico_stdlib hardware_pwm)

pico_add_extra_outputs(homework12)
pico_enable_stdio_usb(homework12 1) 
pico_enable_stdio_uart(homework12 0)
