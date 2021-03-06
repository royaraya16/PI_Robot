/* IMU.c
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

#include "IMU/IMU.h"


static int fd; // file descriptor for the I2C bus

static signed char gyro_orientation[9] = {0,  1,  0,
        -1, 0,  0,
        0,  0,  1};
        
//funcion que se va a ejecutar cada vez que haya datos del IMU disponibles
int (*imu_interrupt_func)();

//esto no deberia estar aqui - hacerse un .c y .h de state
enum state_t state = UNINITIALIZED;

enum state_t get_state(){
	return state;
}

int set_state(enum state_t new_state){
	state = new_state;
	return 0;
}

int null_func(){
	return 0;
}

time_t sec, current_time; // set to the time before calibration

int init_IMU(int calibration_flag){
	
	//set up gpio interrupt pin connected to imu
	if(gpio_export(INTERRUPT_PIN)){
		printf("can't export gpio %d \n", INTERRUPT_PIN);
		return (-1);
	}
	gpio_set_dir(INTERRUPT_PIN, INPUT_PIN);
	gpio_set_edge(INTERRUPT_PIN, "falling");  // Can be rising, falling or both
	
	
    open_bus();
    unsigned char whoami=0;
    i2c_read(MPU6050_ADDR, MPU6050_WHO_AM_I, 1, &whoami);
    
	printf("WHO_AM_I: %x\n", whoami);
    
    struct int_param_s int_param;

	printf("MPU init: %i\n", mpu_init(&int_param));
	printf("MPU sensor init: %i\n", mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
	printf("MPU configure fifo: %i\n", mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
	printf("DMP firmware: %i\n",dmp_load_motion_driver_firmware());
	printf("DMP orientation: %i\n",dmp_set_orientation(
		inv_orientation_matrix_to_scalar(gyro_orientation)));

    unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;

	printf("DMP feature enable: %i\n", dmp_enable_feature(dmp_features));
	printf("DMP set fifo rate: %i\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
	printf("DMP enable %i\n", mpu_set_dmp_state(1));
    

	mpu_set_int_level(1); // Interrupt is low when firing
	dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); // Fire interrupt on new FIFO value

    time(&sec);

    return 0;
}

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char const *data){
    unsigned char tmp[length+1];
    tmp[0] = reg_addr;
    memcpy(tmp+1, data, length);
    if (write(fd, tmp, length+1) != length + 1){
        return -1;
    }
    return 0; 
}
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,
        unsigned char length, unsigned char *data){
    if (write(fd,&reg_addr, 1) != 1){
        return -1;
    }
    if  (read(fd,data, length) != length){
        return -2;
    }

    return 0;
}

int open_bus() { 
    if ((fd = open(BBB_I2C_FILE, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        perror("Failed to open the i2c bus");
        return 1;
    }
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }
    return 0;
}

// multiply two quaternions
int q_multiply(float* q1, float* q2, float* result) {
    float tmp[4];
    tmp[0] = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];
    tmp[1] = q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2];
    tmp[2] = q1[0]*q2[2] - q1[1]*q2[3] + q1[2]*q2[0] + q1[3]*q2[1];
    tmp[3] = q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1] + q1[3]*q2[0];
    memcpy(result, tmp, 4*sizeof(float));
    return 0;
}

// rescale an array of longs by scale factor into an array of floats
int rescale_l(long* input, float* output, float scale_factor, char length) {
    int i;
    for(i=0;i<length;++i)
        output[i] = input[i] * scale_factor;
    return 0;
}

void delay_ms(unsigned long num_ms){

}
void get_ms(unsigned long *count){

}
void reg_int_cb(struct int_param_s *param){

}

inline int min ( int a, int b ){
    return a < b ? a : b;
}
inline void __no_operation(){

}

void euler(float* q, float* euler_angles) {
    euler_angles[0] = -atan2(2*q[1]*q[2] - 2*q[0]*q[3], 2*q[0]*q[0] + 2*q[1]*q[1] - 1); // psi, yaw
    euler_angles[1] = asin(2*q[1]*q[3] + 2*q[0]*q[2]); // phi, pitch
    euler_angles[2] = -atan2(2*q[2]*q[3] - 2*q[0]*q[1], 2*q[0]*q[0] + 2*q[3]*q[3] - 1); // theta, roll
}

// Functions for setting gyro/accel orientation
unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

unsigned short inv_orientation_matrix_to_scalar(
        const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

void advance_spinner() {
    static char bars[] = { '/', '-', '\\', '|' };
    static int nbars = sizeof(bars) / sizeof(char);
    static int pos = 0;
    
	printf("%c\b", bars[pos]);
    
    fflush(stdout);
    pos = (pos + 1) % nbars;
}

int set_imu_interrupt_func(int (*func)(void)){
	imu_interrupt_func = func;
	return 0;
}

void* imu_interrupt_handler(void* ptr){
	
	struct pollfd fdset[1];
	char buf[1];
	int imu_gpio_fd = gpio_fd_open(INTERRUPT_PIN);
	fdset[0].fd = imu_gpio_fd;
	fdset[0].events = POLLPRI; //interrupt de alta prioridad
	
	//While hasta que el programa termine
	
	while(get_state() != EXITING) {
		// system hangs here until IMU FIFO interrupt
		poll(fdset, 1, POLL_TIMEOUT);        
		if (fdset[0].revents & POLLPRI) {
			
			read(fdset[0].fd, buf, MAX_BUF_IMU);
			// user selectable with set_inu_interrupt_func() defined above
			imu_interrupt_func(); 
		}
	}
	
	gpio_fd_close(imu_gpio_fd);
	printf("Saliendo Hilo IMU\n");
	
	return 0;	
}

int mpu6050_read_dmp(mpudata_t *mpu)
{
	short sensors;
	unsigned char more;	
	
	
	float calibratedQuat[4];

	if (dmp_read_fifo(mpu->rawGyro, mpu->rawAccel, mpu->rawQuat, &mpu->dmpTimestamp, &sensors, &more) < 0) {
		return -1;
	}	
	
	rescale_l(mpu->rawQuat, mpu->scaled_rawQuat, QUAT_SCALE, 4);
	
	//Para la calibracion
	
	if(!mpu->calibrated){
		advance_spinner();
		// check if the IMU has finished calibrating
		euler(mpu->scaled_rawQuat, mpu->dmp_euler);
		if(difftime(mpu->current_time, mpu->sec) > 20) {
					
			printf("CALIBRATED! Errors: %.5f %.5f %.5f\n", fabs(mpu->last_euler[0]-mpu->dmp_euler[0]), mpu->last_euler[1]-mpu->dmp_euler[1], mpu->last_euler[2]-mpu->dmp_euler[2]);
			
			// the IMU has finished calibrating
			int i;
			
			mpu->scaled_quat_offset[0] = mpu->scaled_rawQuat[0]; // treat the w value separately as it does not need to be reversed
			
			for(i=1;i<4;++i){
				mpu->scaled_quat_offset[i] = -mpu->scaled_rawQuat[i];
			}
			
			FILE *cal;
			cal = fopen(IMU_CAL_FILE, "w");
			fprintf(cal, "%f\n%f\n%f\n%f\n",  mpu->scaled_rawQuat[0], -mpu->scaled_rawQuat[1], -mpu->scaled_rawQuat[2], -mpu->scaled_rawQuat[3]); 
			fclose(cal);	
			
			mpu->calibrated = 1;
			set_state(EXITING);
		}

		else{
			memcpy(mpu->last_euler, mpu->dmp_euler, 3*sizeof(float));
		}
	}
	
	else{
		q_multiply(mpu->scaled_quat_offset, mpu->scaled_rawQuat, calibratedQuat);
		euler(calibratedQuat, mpu->dmp_euler);		
		mpu->phi = mpu->dmp_euler[1]*180.0/PI;		
	}	

	return 0;
}

int init_IMU_thread(){
	
	set_imu_interrupt_func(&null_func);
	pthread_t imu_interrupt_thread;
	struct sched_param params;
	pthread_create(&imu_interrupt_thread, NULL, imu_interrupt_handler, (void*) NULL);
	params.sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(imu_interrupt_thread, SCHED_FIFO, &params);
	return 0;
}
