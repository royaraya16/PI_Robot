
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
#define IMU_CAL_FILE2	"imu.cal"

 typedef enum flags_t {
	DEBUG_BLUETOOTH,
	CONTROL_REMOTO,
	MOTORES_DESACTIVADOS,
	CALIBRACION,
	AYUDA
} flags_t;

typedef struct {
	
	float Kp;
	float Ki;
	float Kd;
	
	float proporcional;
	float integral;
	float diferencial;
	
	float duty;
	
	float reference;
	float turn;
	float voltage;
	
} pi_robot_t;

mpudata_t mpu; //struct to read IMU data into
pi_robot_t robot;

int PI_flags[5];

int pi_cape_ON();

void ctrl_c(int signo); // signal catcher

int checkProcess();

int pi_cape_OFF();

void parse_args(int argc, char** argv);
void print_usage();

int setPID(float Kp, float Ki, float Kd);
void* send_Serial(void* ptr);
void* battery_monitor(void* ptr);
