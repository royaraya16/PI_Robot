//Codigo para probar los GPIO

#include "Pi_cape/Pi_cape.h"
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol

char *p;
float num;

//variables del IMU
short accel[3], gyro[3], sensors[1];
long quat[4];
unsigned long timestamp;
unsigned char more[0];

int control();

int main(int argc, char *argv[]){
	
	pi_cape_ON();
	
	init_IMU_thread();
	set_imu_interrupt_func(&control);

    while (get_state() != EXITING){
		usleep(100000);
    }
	
	pi_cape_OFF();
	printf("Terminando el main\n");
	return 0;
}

int control(){
	
	if (mpu6050_read_dmp(&mpu) != 0){
				printf("mamo\n");
				return -1;
			}
			
			if(mpu.phi > 75){
				set_state(EXITING);
				printf("saliendo del hilo del IMU\n");
			}
				
			printf("PHI: %5.1f\n", 1.2, mpu.phi);
	
	return 0;	
}
