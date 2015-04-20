
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>		// capture ctrl-c
#include "GPIO/GPIO.h"
#include "PWM/PWM.h"
#include "ADC/ADC.h"
#include "eQEP/eQEP.h"

// lock file location
// file created to indicate running process
// contains pid of current process
#define LOCKFILE "/tmp/robotics.lock"

typedef enum state_t {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	EXITING
} state_t;

enum state_t get_state();
int set_state(enum state_t);

int pi_cape_ON();

void ctrl_c(int signo); // signal catcher

int checkProcess();

int pi_cape_OFF();
