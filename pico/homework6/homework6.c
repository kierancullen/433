#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "font.h"

#define ADC_SCALE 3.3/4096

#define LED_BLINK_MS 250
#define FPS_UPDATE_INTERVAL_MS 50

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    adc_init();
    adc_gpio_init(26); //use ADC on ADC0 pin
    adc_select_input(0);

    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    ssd1306_setup();

    absolute_time_t lastBlink = get_absolute_time();
    int LEDControl = 1;
    char message[100];
    absolute_time_t lastFPSUpdate = get_absolute_time();
    int frames = 0;

    while (1) {

        //display ADC counts
        sprintf(message, "ADC: %.2f V", adc_read()*ADC_SCALE);
        drawMessage(message, 0, 8, 1, 0);
        
        double fps = 1000000.0/(double)absolute_time_diff_us(lastFPSUpdate, get_absolute_time());
        sprintf(message, "FPS: %.2f  ", fps);
        drawMessage(message, 0, 24, 1, 0);
        lastFPSUpdate = get_absolute_time();
        
        //blink the Pico's onboard LED and a pixel on the screen
        if (absolute_time_diff_us(lastBlink, get_absolute_time()) > LED_BLINK_MS*1000) {
            LEDControl = !LEDControl;
            lastBlink = get_absolute_time();
            gpio_put(25,LEDControl);
            //ssd1306_drawPixel(0, 0, LEDCon;trol);
        }

        ssd1306_update(); 
    }

}
