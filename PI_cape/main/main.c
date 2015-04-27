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

char command[30];

float dutyCycle;
float Kp = 0.0667;
int control();

int main(int argc, char *argv[]){

	pi_cape_ON();
	initSerial();
	set_imu_interrupt_func(&control);

    while (get_state() != EXITING){
		usleep(10000);
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
	
	switch(get_state()){
		
		case RUNNING:		
			
			if(mpu.phi < -15 || mpu.phi > 15){
				set_state(PAUSED);
				printf("Pausado\n");
				break;
			}
			
			//debug por terminal
			//printf("%5.1f\n", mpu.phi);
			
			//graficando PHI
			debugSerial(mpu.phi);
			
			dutyCycle = mpu.phi * Kp;
			
			set_motor(2, -dutyCycle);
			set_motor(1, dutyCycle);
			
			ledLogic();
		
			break;
		
		case PAUSED:
		
			disable_motors();
			
			if(mpu.phi > -45 && mpu.phi < 45){
				set_state(RUNNING);
				enable_motors();
				printf("Corriendo\n");
			}
			break;
		
		case EXITING:
			return 0;
			break;
		
		default:
			break;
		}
		
	return 0;	
}
