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
    uint16_t wrap = initPWM(5, 50, 40);

    //generate the desired 4-second trajectory
    int numSteps = DURATION_MS/TIMESTEP_MS;
    float traj[numSteps];
    generateTraj(numSteps, 0, 180, traj);

    //run from 0 degrees to 180 degrees
    for (int i = 0; i < numSteps; i++) {
        holdAngle(5, wrap, traj[i]);
        sleep_ms(TIMESTEP_MS);
    }
    //run from 180 degrees to 0 degrees
    for (int i = numSteps - 1; i >=0; i--) {
        holdAngle(5, wrap, traj[i]);
        sleep_ms(TIMESTEP_MS);
    }

}
