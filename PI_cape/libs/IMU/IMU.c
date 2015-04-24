#include "IMU/IMU.h"
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <time.h>


static int fd; // file descriptor for the I2C bus
static signed char gyro_orientation[9] = {0,  1,  0,
        -1, 0,  0,
        0,  0,  1};
    
        
short accel[3], gyro[3], sensors[1];
long quat[4];
unsigned long timestamp;
unsigned char more[0];

float scaled_quat_offset[4]; 

char command[30];
char str[15];

//static float quad_offset[4]; //buscar el punto de equilibrio del robot y definir el offset aqui
        
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
//function pointers for events initialized to null_func()
//instead of containing a null pointer
//Esto tampoco
int null_func(){
	return 0;
}

int init_IMU(){
	
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
	printf("DMP firmware: %i\n ",dmp_load_motion_driver_firmware());
	printf("DMP orientation: %i\n ",dmp_set_orientation(
		inv_orientation_matrix_to_scalar(gyro_orientation)));

    unsigned short dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;

	printf("DMP feature enable: %i\n", dmp_enable_feature(dmp_features));
	printf("DMP set fifo rate: %i\n", dmp_set_fifo_rate(DEFAULT_MPU_HZ));
	printf("DMP enable %i\n", mpu_set_dmp_state(1));
    

	mpu_set_int_level(1); // Interrupt is low when firing
	dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); // Fire interrupt on new FIFO value
	
	
	//VALORES PARA EL ROBOT EN SU POSICION NEUTRA
	//para conseguirlos, descomentar el codigo en la funcion tratamiento_datos()
	
	scaled_quat_offset[0]=0.739776;
	scaled_quat_offset[1]=-0.111959;
	scaled_quat_offset[2]=0.656173;
	scaled_quat_offset[3]=-0.098151;

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

// rescale an array of shorts by scale factor into an array of floats
int rescale_s(short* input, float* output, float scale_factor, char length) {
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
	printf("terminando IMU interrupt handler\n");
	
	return 0;	
}

int mpu6050_read_dmp(mpudata_t *mpu)
{
	short sensors;
	unsigned char more;

	if (dmp_read_fifo(mpu->rawGyro, mpu->rawAccel, mpu->rawQuat, &mpu->dmpTimestamp, &sensors, &more) < 0) {
		printf("dmp_read_fifo() failed\n");
		//return -1;
	}

	while (more) {
		// Fell behind, reading again
		if (dmp_read_fifo(mpu->rawGyro, mpu->rawAccel, mpu->rawQuat, &mpu->dmpTimestamp, &sensors, &more) < 0) {
			printf("dmp_read_fifo() failed_en More\n");
			return -1;
		}
	}
			
	tratamiento_datos(mpu);
	mpu->phi = mpu->dmp_euler[1]*180.0/PI;

	return 0;
}

int tratamiento_datos(mpudata_t *mpu){
	
	float scaled_rawQuat[4];
	float calibratedQuat[4];
	
	
	rescale_l(mpu->rawQuat, scaled_rawQuat, QUAT_SCALE, 4);
	q_multiply(scaled_quat_offset, scaled_rawQuat, calibratedQuat);
	euler(calibratedQuat, mpu->dmp_euler);
	
	/*printf("raw_quat para el quat_offset: \n");
	for(int i=0; i<4; i++){
		printf("%d: %f ", i, scaled_rawQuat[i]);
	}*/
	
	//sprintf(command, "echo '%s' > /dev/ttyO1\n", str);	
	//system(command);
	
	return 0;
}

int data_fusion(mpudata_t *mpu)
{
	quaternion_t dmpQuat;
	vector3d_t dmpEuler;
	
	dmpQuat[QUAT_W] = (float)mpu->rawQuat[QUAT_W];
	dmpQuat[QUAT_X] = (float)mpu->rawQuat[QUAT_X];
	dmpQuat[QUAT_Y] = (float)mpu->rawQuat[QUAT_Y];
	dmpQuat[QUAT_Z] = (float)mpu->rawQuat[QUAT_Z];

	quaternionNormalize(dmpQuat);	
	quaternionToEuler(dmpQuat, dmpEuler);

	mpu->fusedEuler[VEC3_X] = dmpEuler[VEC3_X];
	mpu->fusedEuler[VEC3_Y] = -dmpEuler[VEC3_Y];
	mpu->fusedEuler[VEC3_Z] = 0;

	eulerToQuaternion(mpu->fusedEuler, mpu->fusedQuat);

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
