#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#define CS_PIN 15
#define PRECISION 1024 //2^10

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

static void writeRegister(bool useDacA, uint16_t dacInput16, uint8_t * src) {
    src[0] = (uint8_t)((dacInput16 << 2) & 0xFF); //fill the lowest two bits with zeros, then take the lowest eight bits
    src[1] = (uint8_t)((dacInput16 >> 6) & 0x0F); //fill the lowest two bits with zeros,, then take only the top four bits

    src[1] = src[1] | (!useDacA << 7); //set bit 15 to 1 if not using DacA, 0 otherwise
    src[1] = src[1] | (0b111 << 4); //always use buffering, gain 1, not shutdown
    
    // csSelect();
    // spi_write_blocking(spi_default, &dacInput, 2);
    // csDeselect();
    // sleep_ms(10);
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
    while (1) {
        float voltage;
        printf("Enter an analog voltage, between 0 and 3.3: ");
        scanf("%f", &voltage);
        printf("\r\n");

        uint8_t src[2];
        uint16_t input = getDacInput16(voltage/3.3);
        writeRegister(0, input, src);
        printf("The 16-bit DAC input is %x \r\n", input);
        printf("The SPI output is %x, %x \r\n", src[0], src[1]);
    }
    
}