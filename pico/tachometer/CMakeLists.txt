add_executable(tachometer)
target_sources(tachometer PRIVATE tachometer.c)

target_link_libraries(tachometer pico_stdlib hardware_irq)

pico_add_extra_outputs(tachometer)
pico_enable_stdio_usb(tachometer 1) 
pico_enable_stdio_uart(tachometer 0)
