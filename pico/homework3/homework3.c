#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#define BOUNCE_DELAY_MS 100

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    adc_init();
    adc_gpio_init(26); //use ADC on ADC0 pin
    adc_select_input(0);

    gpio_init(15);
    gpio_set_dir(15, GPIO_IN);
    gpio_set_pulls(15, 1, 0);
   
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);
    
    //absolute_time_t lastPressTime = get_absolute_time();
    int lastButtonValue = gpio_get(15);
    char message[100];
    while (1) {
        if (gpio_get(15) == 0) { //beginning edge of button press
            //uint16_t result = adc_read();
            //printf("ADC: %d\r\n", result);
            gpio_put(25, 1);
            //sleep_ms(50);
            // if (absolute_time_diff_us(lastPressTime, get_absolute_time()) > 0) { //debouncing delay
            //     lastPressTime = get_absolute_time();
            //     uint16_t result = adc_read();
            //     printf("ADC: %d\r\n", result);
            // }
        }
        else {
            gpio_put(25, 0);  
        }  
        //lastButtonValue = gpio_get(15);
        
    }
}