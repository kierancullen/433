#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

static char event_str[128];
static volatile uint count;
static volatile absolute_time_t lastCountTime;

void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    if (count > 0){
        int64_t elapsedUs = absolute_time_diff_us(lastCountTime, get_absolute_time());
        int64_t revPerSec = 60000000/elapsedUs;
        printf("%d: GPIO %d %s, %d rpm\n", count, gpio, event_str, revPerSec);
    }
    else {
       printf("%d: GPIO %d %s\n", count, gpio, event_str); 
    }

    count++;
    lastCountTime = get_absolute_time();
}

int main() {
    stdio_init_all();
     while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");
    printf("Hello GPIO IRQ\n");
    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Wait forever
    while (1);
}


static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}