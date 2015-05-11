/* Pi_cape.h
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Modifications made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Based on James Strawson's Robotics Cape code.
 * 
 	
Copyright (c) 2015, Roy Araya
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

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

int pi_cape_OFF();

void parse_args(int argc, char** argv);
void print_usage();

float map(float x, float in_min, float in_max, float out_min, float out_max);
int setPID(float Kp, float Ki, float Kd);
int reset_PID();
void* send_Serial(void* ptr);
void* battery_monitor(void* ptr);
void* readSerialControl(void *ptr);
