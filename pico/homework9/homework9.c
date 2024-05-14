#include <stdio.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "hardware/uart.h"
#include "hardware/irq.h"

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
        // //echo back the character
        //     if (uart_is_writable(UART_ID)) {
        //         uart_putc(UART_ID, current);
        // }
        if (current != '\r' && current != '\n' && cursor < 100) {
            buffer[cursor] = current;
            cursor++;
        }
        else { //terminate the string and print it
            buffer[cursor] = '\0'; 
            printf("Pi Zero sent: %s\n",buffer);
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

    //Connect over USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Started.\n");

    char buffer[100];
    while (1) {
        //Get an integer from the PC
        scanf("%s", buffer);
        //Echo it back to the PC
        printf("You entered: %s\r\n", buffer);

        //Send it over serial
        uart_puts(UART_ID, buffer);
    }

}
