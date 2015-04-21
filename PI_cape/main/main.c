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
//Que falta?

float angles[13];

int main(int argc, char *argv[]){
	
	pi_cape_ON();
	
	//Codigo para poder leer el IMU con interrupts---------------------------
  
    struct pollfd fdset[1];
    char buf[1];

    // File descriptor for the GPIO interrupt pin
    int gpio_fd = open(GPIO_INT_FILE, O_RDONLY | O_NONBLOCK);

    //Create an event on the GPIO value file
    memset((void*)fdset, 0, sizeof(fdset));
    fdset[0].fd = gpio_fd;
    fdset[0].events = POLLPRI;

    while (get_state() != EXITING){
		
        poll(fdset, 1, -1);        
            
        if (fdset[0].revents & POLLPRI) {
            // Read the file to make it reset the interrupt
			read(fdset[0].fd, buf, 1);
			
			//Leer MPU
			if (mpu6050_read_dmp(&mpu) != 0){
				printf("mamo\n");
				return -1;
			}
			
			if(mpu.fusedEuler[VEC3_Y]*180.0/PI > 75){
				set_state(EXITING);
				printf("saliendo del hilo del IMU\n");
			}
				
			printf("Dropbone: %5.1f\tStrawson: %5.1f\n", 1.2, mpu.fusedEuler[VEC3_Y]*180.0/PI);
        }
    }
	
	pi_cape_OFF();

	return 0;
}

int control(){
	
	if (mpu6050_read_dmp(&mpu) != 0){
				printf("mamo\n");
				return -1;
			}
			
			if(mpu.fusedEuler[VEC3_Y]*180.0/PI > 75){
				set_state(EXITING);
				printf("saliendo del hilo del IMU\n");
			}
				
			printf("Dropbone: %5.1f\tStrawson: %5.1f\n", 1.2, mpu.fusedEuler[VEC3_Y]*180.0/PI);
	
	return 0;	
}
