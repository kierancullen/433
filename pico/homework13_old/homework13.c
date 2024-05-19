#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "MPU6050M.h"

#define LED_BLINK_MS 250
#define DATA_PRINT_MS 100

const char *labels[] = {
    "accel_x",
    "accel_y",
    "accel_z",
    "temperature",
    "gyro_x",
    "gyro_y",
    "gyro_z"
};
void printIMUData() {
    int16_t result[7];
    double resultConverted[7];
    MPU_readAll(result);

    resultConverted[0] = result[0] * 0.000061;
    resultConverted[1] = result[1] * 0.000061;
    resultConverted[2] = result[2] * 0.000061;
    resultConverted[3] = result[3] / 340.00 + 36.53;
    resultConverted[4] = result[4] * 0.007630;
    resultConverted[5] = result[5] * 0.007630;
    resultConverted[6] = result[6] * 0.007630;

    for (int i = 0; i < 7; i++) {
        printf("%-15s : %-15.3f\n", labels[i], resultConverted[i]);
    }
    
}

int main() {
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

    absolute_time_t lastBlink = get_absolute_time();
    absolute_time_t lastDataPrint = get_absolute_time();
    int LEDControl = 0;
    gpio_put(25,LEDControl); //turn off LED on to begin with 

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    int success = MPU_setup();

    if (!success) {
        while (1) {
            sleep_ms(100);
            gpio_put(25, 1); //turn on the LED to indicate a problem
        }
    }

    while (1) {
        //blink the Pico's onboard LED 
        if (absolute_time_diff_us(lastBlink, get_absolute_time()) > LED_BLINK_MS*1000) {
            LEDControl = !LEDControl;
            lastBlink = get_absolute_time();
            gpio_put(25,LEDControl);
        }

        if (absolute_time_diff_us(lastDataPrint, get_absolute_time()) > DATA_PRINT_MS*1000) {
            printf("\033c");
            // uint8_t result[2];
            // MPU_read(ACCEL_ZOUT_H, result, 2);
            // printf("z acceleration: 0x%x 0x%x end \r\n", result[0], result[1]);
            // uint16_t combined = (result[0] << 8) | result[1];
            // double accelResult = ((int16_t)combined) / 16384;
            // printf("z acceleration: %.6f \r\n", accelResult);
            printIMUData();
            
            lastDataPrint = get_absolute_time();
        }
    }
}
