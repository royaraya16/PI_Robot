#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "GPIO/SimpleGPIO.h"
#include "PWM/tipwmss.h"	// pwmss and eqep registers

int set_motor(int motor, float duty);
int kill_pwm();
int enable_motors();
int disable_motors();
