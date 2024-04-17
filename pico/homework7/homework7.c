#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "font.h"

#include "hardware/uart.h"
#include "hardware/irq.h"

#define LED_BLINK_MS 250

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

static volatile int cursor = 0;
static char buffer[101];

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        char current = uart_getc(UART_ID);
        //echo back the character
            if (uart_is_writable(UART_ID)) {
                uart_putc(UART_ID, current);
        }
        if (current != '\r' && current != '\n' && cursor < 100) {
            buffer[cursor] = current;
            cursor++;
        }
        else { //terminate the string and print it
            buffer[cursor] = '\0'; 
            ssd1306_clear();
            drawMessage(buffer, 0, 0, 1, 1);
            cursor = 0;
        }
    }
}

int main() {
    stdio_init_all();

    //onboard LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    ssd1306_setup();

    absolute_time_t lastBlink = get_absolute_time();
    int LEDControl = 1;
    char message[100];

    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    while (1) {
        
        //blink the Pico's onboard LED and a pixel on the screen
        if (absolute_time_diff_us(lastBlink, get_absolute_time()) > LED_BLINK_MS*1000) {
            LEDControl = !LEDControl;
            lastBlink = get_absolute_time();
            gpio_put(25,LEDControl);
            //ssd1306_drawPixel(0, 0, LEDControl);
        }

        ssd1306_update(); 
    }

}
