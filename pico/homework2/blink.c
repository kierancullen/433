#include "pico/stdlib.h"
#define DELAY_MS 100 //100ms per half-cycle => 5 on/off cycles per second
//For onboard LED:
#define LED_PIN 25

int main() {

    gpio_init(LED_PIN);

    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(DELAY_MS); 
        gpio_put(LED_PIN, 0);
        sleep_ms(DELAY_MS);
    }
}