#include <stdio.h>
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define CLOCK 125000000
#define MIN_DUTY_CYCLE_DIV 40 //2.5% min duty cycle
#define MAX_DUTY_CYCLE_DIV 8 //12.5% max duty cycle
#define DURATION_MS 2000 //duration of the trajectory
#define TIMESTEP_MS 10 //length of each time step in the trajectory

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

//generate a linear angle trajectory as an array of angles
static void generateTraj(int steps, float minAngleDeg, float maxAngleDeg, float* traj) {
    float angleStep = (maxAngleDeg - minAngleDeg)/(steps-1);
    for (int i = 0; i < steps; i++) {
        traj[i] = minAngleDeg + (angleStep * i);
    }
}

int main() {
    stdio_init_all();

    gpio_init(18);
    gpio_set_dir(18, GPIO_IN);
    gpio_set_pulls(18, 1, 0);

    float angleDegs[] = {110, 95, 90};

    uint16_t wrap1 = initPWM(5, 50, 40);
    uint16_t wrap2 = initPWM(13, 50, 40);
    holdAngle(5, wrap1, 110);
    
    int angleIndex = 0;
    while(1) {
        holdAngle(5, wrap1, angleDegs[angleIndex]);
        if (gpio_get(18) == 0) {
            sleep_ms(20);
            if (gpio_get(18) == 0) {
                while (gpio_get(18) == 0) {
                    ;
                }
                angleIndex++;
                if (angleIndex > 2) {
                    angleIndex = 0;
                }
            }
        }
    }
}
