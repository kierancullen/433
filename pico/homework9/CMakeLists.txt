add_executable(homework9)

target_sources(homework9 PRIVATE homework9.c)

target_link_libraries(homework9 pico_stdlib hardware_uart hardware_irq)

pico_add_extra_outputs(homework9)
pico_enable_stdio_usb(homework9 1) 
pico_enable_stdio_uart(homework9 0)
