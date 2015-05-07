
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

 typedef enum controlPID_t {
	WAITING,
	SP,
	SI,
	SD,
	ST
} controlPID_t;


typedef struct {
	
	float Kp;
	float Ki;
	float Kd;
	
	float proporcional;
	float integral;
	float diferencial;
	
	float duty;
	
	float error;
	float last_error;
	
	float reference;
	float turn;
	float voltage;
	
	char buffer[32];
	
} pi_robot_t;

mpudata_t mpu; //struct to read IMU data into
pi_robot_t robot;

//arreglos para usar las estructuras
int PI_flags[5]; //usado en los flags que se mandan al iniciar el programa

int pi_cape_ON();

void ctrl_c(int signo); // signal catcher

int checkProcess();

int pi_cape_OFF();

void parse_args(int argc, char** argv);
void print_usage();

float map(float x, float in_min, float in_max, float out_min, float out_max);
int setPID(float Kp, float Ki, float Kd);
int reset_PID();
void* send_Serial(void* ptr);
void* battery_monitor(void* ptr);
void* readSerialControl(void *ptr);
