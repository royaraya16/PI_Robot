
#include "GPIO/SimpleGPIO.h"
#include "PWM/tipwmss.h"	// pwmss and eqep registers

int set_motor(int motor, float duty);
int kill_pwm();
int enable_motors();
int disable_motors();
