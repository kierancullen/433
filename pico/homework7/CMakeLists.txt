add_executable(homework7)

target_sources(homework7 PRIVATE homework7.c ssd1306.c font.c)

target_link_libraries(homework7 pico_stdlib hardware_i2c hardware_uart hardware_irq)

pico_add_extra_outputs(homework7)
