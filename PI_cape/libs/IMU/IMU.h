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

#define THRESHOLD (0.1*PI/180.0) // the amount that the IMU Euler values have to change less than to indicate that calibration has finished

#define CALIBRATION_TIME 20.0

#define DEG_TO_RAD 		0.0174532925199
#define RAD_TO_DEG 	 	57.295779513

//#define MAX_BUF 64
#define MAX_BUF_IMU 1 //en dropbone estaba en 1

#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "GPIO/SimpleGPIO.h"
#include "IMU/quaternion.h"
#include <pthread.h>



/* The following functions must be defined for this platform:
 * i2c_write(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char const *data)
 * i2c_read(unsigned char slave_addr, unsigned char reg_addr,
 *      unsigned char length, unsigned char *data)
 * delay_ms(unsigned long num_ms)
 * get_ms(unsigned long *count)
 * reg_int_cb(void (*cb)(void), unsigned char port, unsigned char pin)
 * labs(long x)
 * fabsf(float x)
 * min(int a, int b)
 */
 
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
	
	float last_euler[3];
	
	float dmp_euler[4];
	unsigned long dmpTimestamp;

	quaternion_t fusedQuat;
	vector3d_t fusedEuler;

	float phi;
	float last_phi;
	float duty;
	
	int calibrated;
	
} mpudata_t;

int init_IMU(int calibration_flag);

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char const *data);
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char *data);

int open_bus();

int q_multiply(float* q1, float* q2, float* result); // multiply two quaternions together {w, x, y, z}

int rescale_l(long* input, float* output, float scale_factor, char length);
int rescale_s(short* input, float* output, float scale_factor, char length);
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
int tratamiento_datos(mpudata_t *mpu);
int data_fusion(mpudata_t *mpu);
int init_IMU_thread();
