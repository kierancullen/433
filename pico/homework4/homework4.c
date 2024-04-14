#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#define CS_PIN 15
#define PRECISION 1024 //2^10
#define FREQ_SIN 2000 //sine wave frequency
#define FREQ_TRI 1000 //triangle wave frequency
#define UPDATE_FREQ 100000 //frequency at which to send new voltages to the DAC

static inline void csSelect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(CS_PIN, 0);  //For active low
    asm volatile("nop \n nop \n nop");
}

static inline void csDeselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(CS_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static void writeRegister(bool useDacA, uint16_t dacInput16) {
    uint8_t src[2];
    src[1] = (uint8_t)((dacInput16 << 2) & 0xFF); //fill the lowest two bits with zeros, then take the lowest eight bits
    src[0] = (uint8_t)((dacInput16 >> 6) & 0x0F); //fill the lowest two bits with zeros,, then take only the top four bits

    src[0] = src[0] | (!useDacA << 7); //set bit 15 to 1 if not using DacA, 0 otherwise
    src[0] = src[0] | (0b111 << 4); //always use buffering, gain 1, not shutdown
    
    csSelect();
    spi_write_blocking(spi_default, src, 2);
    csDeselect();
    sleep_ms(10);
}

static uint16_t getDacInput16 (float refRatio) { //refRatio is <desired vOut> / <reference voltage>
    return (uint16_t) ((PRECISION-1)*refRatio); 

}

int main() {
    stdio_init_all();
    spi_init(spi_default, 500000);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    gpio_init(CS_PIN); //for chip select
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);
    
     while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Started.\n");

    int resolutionSin = UPDATE_FREQ / FREQ_SIN;
    int resolutionTri = UPDATE_FREQ / FREQ_TRI;
    uint16_t inputsSin[resolutionSin];
    uint16_t inputsTri[resolutionTri];

    for (int i = 0; i < resolutionSin; i++) {
        inputsSin[i] = getDacInput16((sin(i*2*3.1415/resolutionSin) + 1)/2);
    }

    for (int i = 0; i < resolutionTri; i++) {
        if (i < resolutionTri/2) {
            inputsTri[i] = getDacInput16(2.0*i/resolutionTri);
        }
        else {
            inputsTri[i] = getDacInput16(1.0-(2.0*(i-resolutionTri/2)/resolutionTri));  
        }
    }

    printf("Done computing points.\n");
    for (int i = 0; i < resolutionTri; i++) {
        printf("Data point %d: %d\r\n", i, inputsTri[i]);
        sleep_ms(10);
    }

    int sinIndex = 0;
    int triIndex = 0;
    int delayNs = (1000000/UPDATE_FREQ);
    while (1) {
        writeRegister(1, inputsSin[sinIndex]);
        writeRegister(0, inputsTri[triIndex]);
        sinIndex++;
        triIndex++;
        if (sinIndex >= resolutionSin) {
            sinIndex = 0;
        }
        if (triIndex >= resolutionTri) {
            triIndex = 0;
        }
        sleep_ms(delayNs);
    }
}