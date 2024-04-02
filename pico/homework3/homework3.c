#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#define ADC_DELAY_MS 10 //for 100Hz read frequency
#define ADC_SCALE 3.3/4096

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    adc_init();
    adc_gpio_init(26); //use ADC on ADC0 pin
    adc_select_input(0);

    gpio_init(0);
    gpio_set_dir(0, GPIO_IN);
    gpio_set_pulls(0, 1, 0);

    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);
    while (1) {
        gpio_put(15, 1); //LED on
    
        while (gpio_get(0) != 0) { //wait for button to be low (pressed)
            ;
        }

        gpio_put(15, 0); //LED off

        //Read the number of samples to take
        int sampleCount;
        printf("Enter a number of analog voltage samples to take, between 1 and 100: ");
        scanf("%d", &sampleCount);
        printf("\r\n");
        
        //Collect the samples
        uint16_t resultsRaw[sampleCount];
        int i = 0;
        while (i < sampleCount) {
            resultsRaw[i] = adc_read();
            i++;
            sleep_ms(ADC_DELAY_MS);
        }

        //Print the samples back to the terminal
        for (i = 0; i < sampleCount; i++) {
            printf("Sample %d: %.3f V\r\n", i+1, resultsRaw[i]*ADC_SCALE);
        }
        
    } 
}