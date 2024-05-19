#include <stdio.h>
#include "MPU6050M.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

unsigned char DEVICE_ADDRESS = 0x68; //7-bit I2C address

//write a command to an address
static void MPU_command(unsigned char address, unsigned char command) {
    uint8_t buf[2]; 
    buf[0] = address; 
    buf[1] = command;
    i2c_write_blocking(i2c_default, DEVICE_ADDRESS, buf, 2, false);
}

//read from the register at an address
void MPU_read(unsigned char address, uint8_t* result, int length) {
    uint8_t buf[1]; 
    buf[0] = address; 
    i2c_write_blocking(i2c_default, DEVICE_ADDRESS, buf, 1, true);
    i2c_read_blocking(i2c_default, DEVICE_ADDRESS, result, length, false);
}

void MPU_readAll(int16_t* result) {
    uint8_t buf[14];

    MPU_read(ACCEL_XOUT_H, buf, 14); //read all the data bits at once
    for (int i = 0; i < 7; i++) { //combine the 8-bit blocks
        uint16_t combined = (buf[2*i] << 8) | buf[2*i+1];
        result[i] = (int16_t)combined;
    }
}

int MPU_setup() {
    MPU_command(PWR_MGMT_1, 0x00); 
    MPU_command(ACCEL_CONFIG, 0x00); //set AFS_SEL to +-2g
    MPU_command(GYRO_CONFIG, 0x18); //set FS_SEL to +-2000 deg/s

    uint8_t identifier;
    MPU_read(WHO_AM_I, &identifier, 1); 
    if (identifier == 0x68) {
        return 1;
    }
    else {
        return 0;
    }
    //printf("The identifier is 0x%x", identifier);
}







// // update every pixel on the screen
// void ssd1306_update() {
//     ssd1306_command(SSD1306_PAGEADDR);
//     ssd1306_command(0);
//     ssd1306_command(0xFF);
//     ssd1306_command(SSD1306_COLUMNADDR);
//     ssd1306_command(0);
//     ssd1306_command(128 - 1); // Width

//     unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
//     unsigned char * ptr = ssd1306_buffer; // first address of the pixel buffer
//     /*
//     i2c_master_start();
//     i2c_master_send(ssd1306_write);
//     i2c_master_send(0x40); // send pixel data
//     // send every pixel
//     while (count--) {
//         i2c_master_send(*ptr++);
//     }
//     i2c_master_stop();
//     */

//     i2c_write_blocking(i2c_default, SSD1306_ADDRESS, ptr, 513, false);
// }

// // set a pixel value. Call update() to push to the display)
// void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) {
//     if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) {
//         return;
//     }

//     if (color == 1) {
//         ssd1306_buffer[1 + x + (y / 8)*128] |= (1 << (y & 7));
//     } else {
//         ssd1306_buffer[1 + x + (y / 8)*128] &= ~(1 << (y & 7));
//     }
// }

// // zero every pixel value
// void ssd1306_clear() {
//     memset(ssd1306_buffer, 0, 512); // make every bit a 0, memset in string.h
//     ssd1306_buffer[0] = 0x40; // first byte is part of command
// }
