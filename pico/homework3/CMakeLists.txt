add_executable(homework3)

target_sources(homework3 PRIVATE homework3.c)

target_link_libraries(homework3 pico_stdlib hardware_adc)

pico_add_extra_outputs(homework3)

pico_enable_stdio_usb(homework3 1) 
pico_enable_stdio_uart(homework3 0)