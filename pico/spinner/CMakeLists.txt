add_executable(spinner)
pico_generate_pio_header(spinner ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)
target_sources(spinner PRIVATE spinner.c)

target_link_libraries(spinner pico_stdlib pico_multicore hardware_pio hardware_pwm hardware_uart hardware_irq)

pico_add_extra_outputs(spinner)
pico_enable_stdio_usb(spinner 1) 
pico_enable_stdio_uart(spinner 0)
