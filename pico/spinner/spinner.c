#include <stdio.h>
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#include "quadrature_encoder.pio.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define CPR 1440.0
#define MIN_POWER 5
#define MAX_POWER 10

#define CLOCK 125000000
#define MIN_DUTY_CYCLE_DIV 40 //2.5% min duty cycle
#define MAX_DUTY_CYCLE_DIV 8 //12.5% max duty cycle

static char event_str[128];
static volatile uint count;
static volatile absolute_time_t lastCountTime;

void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    if (count > 0){
        int64_t elapsedUs = absolute_time_diff_us(lastCountTime, get_absolute_time());
        int64_t revPerSec = 60000000/elapsedUs;
        printf("%d: GPIO %d %s, %d rpm\n", count, gpio, event_str, revPerSec);
    }
    else {
       printf("%d: GPIO %d %s\n", count, gpio, event_str); 
    }

    count++;
    lastCountTime = get_absolute_time();
}

static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}

//set up PWM output on one pin
static uint16_t initPWM(int pin, int freq, float div) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice_num, div); 
    uint16_t wrap = (uint16_t) (CLOCK/(freq*div)); 
    pwm_set_wrap(slice_num, wrap-1);
    pwm_set_enabled(slice_num, true);   //turn on the PWM
    return wrap;
    
}

//have the servo hold an angle
static void holdAngle(int pin, int wrap, float angleDeg) {
    angleDeg = (angleDeg > 180) ? 180 : (angleDeg < 0 ? 0 : angleDeg); //clip the angle to be in the range [0,180]
    uint16_t level = (uint16_t)((wrap/MIN_DUTY_CYCLE_DIV) + (wrap/MAX_DUTY_CYCLE_DIV - wrap/MIN_DUTY_CYCLE_DIV) * angleDeg/180);
    pwm_set_gpio_level(pin, level); 
}

//apply some motor power via the ESC
static void holdPower(int pin, int wrap, float power) {
    power = (power > 100) ? 100 : (power < 0 ? 0 : power); //clip the power to be in the range [0,100]
    uint16_t level = (uint16_t)(wrap * power/100);
    pwm_set_gpio_level(pin, level); 
}

static volatile int cursor = 0;
static char buffer[101];
static volatile float targetSpeed = 0;
static uint16_t wrap1, wrap2;
static int mark;

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
            float got1, got2;
            sscanf(buffer, "(%f,%f,%d)", &got1, &got2, &mark); 
            // printf("Got motor power: %f\n",got1);
            // printf("Got servo angle: %f\n",got2);
            targetSpeed = got1;
            holdAngle(5, wrap1, got2);
            cursor = 0;
        }
    }
}

void init_uart() {
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
}

int main() {
    stdio_init_all();

    init_uart();
    //onboard LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    float angleDegs[] = {110, 95, 90};

    wrap1 = initPWM(5, 50, 40);
    wrap2 = initPWM(15, 50, 40);
    holdAngle(5, wrap1, 110);
    holdPower(15, wrap2, 1.0);

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB = 10;
    PIO pio = pio0;
    const uint sm = 0;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    pio_add_program(pio, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio, sm, PIN_AB, 0);
    
     while (!stdio_usb_connected()) {
         sleep_ms(100);
     }
     printf("c\r\n");

    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    int angleIndex = 0;
    
    int currentCounts = 0;
    absolute_time_t lastTime = get_absolute_time();
    double currentSpeed;
    float currentTurns, lastTurns;
    float minPower, maxPower;
    float lastError;
    double timeElapsed = 0;

    double startTurns = 0;
    absolute_time_t sampleStartTime;
    int recording = 0;
    while(1) {
        currentCounts = -quadrature_encoder_get_count(pio, sm);
        currentTurns = ((float)currentCounts)/CPR;
        currentSpeed = 60.0*(currentTurns-lastTurns)/(absolute_time_diff_us(lastTime, get_absolute_time())/1000000.0);
        //timeElapsed += absolute_time_diff_us(lastTime, get_absolute_time()) / 1000000.0;
        
        double error = targetSpeed-currentSpeed;
        double setPoint =  (targetSpeed > 0) ? 7 : 0; //MIN_POWER+(targetSpeed/10000.0)*(MAX_POWER-MIN_POWER); //
        double P = 0.00005*(error);
        double D = 20000000000000*(error - lastError)/(absolute_time_diff_us(lastTime, get_absolute_time())/1000000.0);
        
        lastTime = get_absolute_time();
        lastTurns = currentTurns;
         
        float control = setPoint + P;
        if (targetSpeed > 0) {
            control = (control > MAX_POWER) ? MAX_POWER : (control < (MIN_POWER+1) ? (MIN_POWER+1) : control); //clip the power to be in the range [MIN_POWER,MAX_POWER]
        }
        holdPower(15, wrap2, control);
        char output[30];
        if (mark) {
            mark = 0;
            recording = 1;
            startTurns = currentTurns;
            sampleStartTime = get_absolute_time();
        }
        if (recording && (absolute_time_diff_us(sampleStartTime, get_absolute_time())/1000000.0) > 0.5) {
            double speed = 60*(currentTurns - startTurns)/(absolute_time_diff_us(sampleStartTime, get_absolute_time())/1000000.0);
            sprintf(output, "%.3f\n", speed);
            uart_puts(UART_ID, output);
            recording = 0;
        }

        // printf("\033c");
        // printf("Target speed: %.2f \r\n", targetSpeed);
        // printf("Speed: %.2f rpm\r\n", currentSpeed);
        // printf("Setpoint %.2f \r\n", setPoint);
        // printf("Control %.2f \r\n", control);
        //printf("Turns %.2f \r\n", currentTurns);
        //printf("Time elapsed %.6f \r\n", timeElapsed);

        // lastCounts = currentCounts;
        
        // lastError = error;
        // lastTurns = currentTurns;
        // printf("Turns: %.2f\r\n", currentTurns);
        // scanf("(%f,%f)", &angleDeg, &speed);
        // holdSpeed(15, wrap2, speed);
        // holdAngle(5, wrap1, angleDeg);
        // holdAngle(5, wrap1, angleDegs[angleIndex]);
        // if (gpio_get(BUTTON_PIN) == 0) {
        //     sleep_ms(20);
        //     if (gpio_get(BUTTON_PIN) == 0) {
        //         while (gpio_get(BUTTON_PIN) == 0) {
        //             ;
        //         }
        //         angleIndex++;
        //         if (angleIndex > 2) {
        //             angleIndex = 0;
        //         }
        //     }
        // }

        // holdSpeed(15, wrap2, speeds[angleIndex]);
        // if (gpio_get(BUTTON_PIN) == 0) {
        //     sleep_ms(20);
        //     if (gpio_get(BUTTON_PIN) == 0) {
        //         while (gpio_get(BUTTON_PIN) == 0) {
        //             ;
        //         }
        //         angleIndex++;
        //         if (angleIndex > 2) {
        //             angleIndex = 0;
        //         }
        //     }
        // }  
        

        // printf("position %8d, delta %6d\n", new_value, delta);
        // sleep_ms(100);
    }
}
