

#include <string.h>
#include "GPIO/GPIO.h"
#include "GPIO/SimpleGPIO.h"

int init_PWM();
int set_motor(int motor, float duty);
int kill_pwm();
int enable_motors();
int disable_motors();
