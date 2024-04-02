#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    adc_init();
    adc_gpio_init(26); //use ADC on ADC0 pin
    adc_select_input(0);
 
    while (1) {
        char message[100];
        scanf("%s", message);
        printf("message: %s\r\n",message);
        sleep_ms(50);
        uint16_t result = adc_read();
        printf("adc value: %d\r\n", &result);
    }
}