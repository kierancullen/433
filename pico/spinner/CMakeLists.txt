add_executable(spinner)

target_sources(spinner PRIVATE spinner.c)

target_link_libraries(spinner pico_stdlib hardware_pwm)

pico_add_extra_outputs(spinner)
pico_enable_stdio_usb(spinner 1) 
pico_enable_stdio_uart(spinner 0)
