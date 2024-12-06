add_executable(homework16)

target_sources(homework16 PRIVATE homework16.c)

target_link_libraries(homework16 pico_stdlib hardware_pwm hardware_uart hardware_irq)

pico_add_extra_outputs(homework16)
pico_enable_stdio_usb(homework16 1) 
pico_enable_stdio_uart(homework16 0)
