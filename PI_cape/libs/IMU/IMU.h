/* IMU.h
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Based on Team 4774's Dropbone IMU code.
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

#define MPU6050 // The Invensense Motion Driver code needs this

#define BBB_I2C_FILE "/dev/i2c-1"

#define MPU6050_ADDR 0x68
#define MPU6050_WHO_AM_I 0x75

#define DEFAULT_MPU_HZ 200

#define CALIBRATION_DEBUG 0

#define PI 3.14159
#define QUAT_SCALE (1.0/1073741824)

#define GPIO_INT_FILE "/sys/class/gpio/gpio117/value" 

#define INTERRUPT_PIN 117  //gpio3.21 P9.25

#define GYRO_SCALE (PI/(180.0*16.384))

#define ACCEL_SCALE (1.0/16384)

#define CALIBRATION_TIME 20.0

#define DEG_TO_RAD 		0.0174532925199
#define RAD_TO_DEG 	 	57.295779513

#define IMU_CAL_FILE	"imu.cal"

#define MAX_BUF_IMU 1 //en dropbone estaba en 1

#include "GPIO/SimpleGPIO.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <time.h>
#include <pthread.h>
 
 typedef enum state_t {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	EXITING
} state_t;

enum state_t get_state();
int set_state(enum state_t);
int null_func();

#define log_i printf

typedef struct {
	short rawGyro[3];
	short rawAccel[3];
	long rawQuat[4];
	float scaled_rawQuat[4];
	
	float scaled_quat_offset[4]; 
	
	float last_euler[3];
	
	float dmp_euler[4];
	unsigned long dmpTimestamp;

	float phi;
	float last_phi;
	
	int calibrated;
	
	time_t sec, current_time;
	
} mpudata_t;

int init_IMU(int calibration_flag);

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char const *data);
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char *data);

int open_bus();

int q_multiply(float* q1, float* q2, float* result); // multiply two quaternions together {w, x, y, z}

int rescale_l(long* input, float* output, float scale_factor, char length);
void delay_ms(unsigned long num_ms);
void get_ms(unsigned long *count);
void reg_int_cb(struct int_param_s *);

void parse_args(int argc, char** argv);
void print_usage();

inline int min ( int a, int b );
inline void __no_operation();

void euler(float* q, float* euler_angles); // Convert quaternions to Euler angles

// Functions for setting gyro/accel orientation
unsigned short inv_row_2_scale(const signed char *row);
unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx);

void advance_spinner();

int set_imu_interrupt_func(int (*func)(void));
void* imu_interrupt_handler(void* ptr);

int data_ready();
int mpu6050_read_dmp(mpudata_t *mpu);
int data_fusion(mpudata_t *mpu);
int init_IMU_thread();
