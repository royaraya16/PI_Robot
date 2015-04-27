
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>		// capture ctrl-c
#include "GPIO/GPIO.h"
#include "PWM/PWM.h"
#include "ADC/ADC.h"
#include "eQEP/eQEP.h"
#include "IMU/IMU.h"
#include "Serial/Serial.h"
#include <poll.h>
#include <time.h>
#include <math.h>		// atan2 and fabs

// lock file location
// file created to indicate running process
// contains pid of current process
#define LOCKFILE "/tmp/robotics.lock"

mpudata_t mpu; //struct to read IMU data into

int pi_cape_ON();

void ctrl_c(int signo); // signal catcher

int checkProcess();

int pi_cape_OFF();
