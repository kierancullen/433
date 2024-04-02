add_executable(blink)

target_sources(blink PRIVATE blink.c)

target_link_libraries(blink pico_stdlib)

pico_add_extra_outputs(blink)