#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "font.h"

#define LED_BLINK_MS 250

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);


    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    ssd1306_setup();

     while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    absolute_time_t last = get_absolute_time();
    int LEDControl = 1;
    char message[100];
    sprintf(message, "The quick brown fox jumped over the lazy dogs.");
    drawMessage(message, 0, 1, 0, 0);
    while (1) {

        //blink the Pico's onboard LED and a pixel on the screen
        if (absolute_time_diff_us(last, get_absolute_time()) > LED_BLINK_MS*1000) {
            LEDControl = !LEDControl;
            last = get_absolute_time();
            gpio_put(25,LEDControl);
            ssd1306_drawPixel(0, 0, LEDControl);
            ssd1306_update();
        } 
    
    }

}
