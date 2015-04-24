//Codigo para probar los GPIO

#include "Pi_cape/Pi_cape.h"
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>

char *p;
float num;

//variables del IMU
short accel[3], gyro[3], sensors[1];
long quat[4];
unsigned long timestamp;
unsigned char more[0];

int i;

int control();

int main(int argc, char *argv[]){
	
	pi_cape_ON();
	
	init_IMU_thread();
	set_imu_interrupt_func(&control);
	enable_motors();

    while (get_state() != EXITING){
		usleep(100000);
    }
	
	pi_cape_OFF();
	printf("Terminando el main\n");
	return 0;
}

int control(){
	
	if (mpu6050_read_dmp(&mpu) != 0){
				printf("Error leyendo DMP\n");
				return -1;
	}
			
	/*if(mpu.phi < -1){
		set_state(EXITING);
		printf("saliendo del hilo del IMU\n");
	}

	rescale_l(mpu.rawQuat, mpu.scaled_rawQuat, QUAT_SCALE, 4);
	q_multiply(scaled_quad_offset, mpu.scaled_rawQuat, mpu.calibratedQuat);
	euler(mpu.calibratedQuat, mpu.angles);*/
	
	//DEBUG POR BLUETOOTH
	printf("%5.1f\n", mpu.phi);
	
	
	/*printf("raw_quat: \n");
	for(i=0; i<4; i++){
		printf("%d: %f\n", i, mpu.angles[i]*180.0/PI);
	}*/
	
	//set_motor(2, -0.5);
	
	//logica para los LEDS
	
	return 0;	
}
