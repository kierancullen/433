#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define LED_BLINK_MS 250
 // device address
#define ADDR _u(0b0100000) //maybe not supplied as a 7-bit number

// hardware registers
#define REG_IODIR _u(0x00)
#define REG_GPIO _u(0x09)
#define REG_OLAT _u(0x0A)

void init() {
    uint8_t buf[2];
    buf[0] = REG_IODIR;
    buf[1] = 0b01111111;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

void setPins(uint8_t OLATbits) {
    uint8_t buf[2];

    buf[0] = REG_OLAT;
    buf[1] = OLATbits;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

uint8_t readPins() {
    uint8_t buf[1];
    uint8_t reg = REG_GPIO;
    i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDR, buf, 1, false);
    return buf[0];
}

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN); //use these instead of pull-up resistors on the board 

    //initialize the MCP23008
    init();
    sleep_ms(250);

    setPins(0b0); //GPIO7 off

    absolute_time_t last = get_absolute_time();
    int LEDControl = 1;
    while (1) {
        
        //If the button is pressed (GPIO0 is low), make GPIO7 high
        if (((int)readPins() & 0b1) == 0) {
            setPins(0b10000000);
        }
        else {
            setPins(0b00000000);
        }

        //blink the Pico's onboard LED
        if (absolute_time_diff_us(last, get_absolute_time()) > LED_BLINK_MS*1000) {
            LEDControl = !LEDControl;
            last = get_absolute_time();
            gpio_put(25,LEDControl);
        } 

    }

}
