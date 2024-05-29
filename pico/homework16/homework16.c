#include <stdio.h>
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#define CLOCK 125000000

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

static volatile int cursor = 0;
static char buffer[101];
static uint16_t wrap;

//set up PWM output on a pin
static uint16_t initPWM(int pin, int freq, float div) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice_num, div); 
    uint16_t wrap = (uint16_t) (CLOCK/(freq*div)); 
    pwm_set_wrap(slice_num, wrap-1);
    pwm_set_enabled(slice_num, true);   //turn on the PWM
    return wrap;
    
}

//have set the duty cycle on a PWM output, as a percentage
static void setDutyCyclePercent(int pin, int wrap, float percent) {
    percent = (percent > 100) ? 100 : (percent < 0 ? 0 : percent); //clip the duty cycle to be in the range [0,100]
    uint16_t level = (uint16_t)(wrap * percent/100);
    pwm_set_gpio_level(pin, level); 
}

static void setMotorPower(int pwmPin, int dirPin, float percent) {
    percent = (percent > 100) ? 100 : (percent < -100 ? -100 : percent); //clip the power to be in the range [-100,100]
    if (percent >= 0) {
        gpio_put(dirPin, 0); 
        setDutyCyclePercent(pwmPin, wrap, percent);
    }
    else {
        gpio_put(dirPin, 1); 
        setDutyCyclePercent(pwmPin, wrap, 100+percent);
    }
}

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        char current = uart_getc(UART_ID);

        if (current != '\r' && current != '\n' && cursor < 100) {
            buffer[cursor] = current;
            cursor++;
        }
        else { //terminate the string and print it
            buffer[cursor] = '\0';
            int got1, got2;
            sscanf(buffer, "(%d,%d)", &got1, &got2); 
            // printf("Got left power: %d\n",got1);
            // printf("Got right power: %d\n",got2);
            setMotorPower(16, 17, got1);
            setMotorPower(18, 19, got2);
            cursor = 0;
        }
    }

}

int main() {
    stdio_init_all();

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

    // //Connect over USB
    // while (!stdio_usb_connected()) {
    //     sleep_ms(100);
    // }

    // printf("Started.\n");

    gpio_init(17);
    gpio_set_dir(17, GPIO_OUT);
    gpio_put(17, 0);

    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_put(19, 0);
    
    wrap = initPWM(16, 50, 40);
    initPWM(18, 50, 40);
    setMotorPower(16, 17, 0);
    setMotorPower(18, 19, 0);

    while (1) {
        ; //just wait for interrupts
    }

}
