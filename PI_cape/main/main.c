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



float Kp = 0.135;
float Ki = 0.005;
float Kd = 0.02;

int control();
void* send_Serial(void* ptr);
char uart1_directoryz[30] = "/dev/ttyO1";

pthread_mutex_t mpu_mutex;

float proporcional;
float integral = 0;
float diferencial;

int main(int argc, char *argv[]){
	parse_args(argc, argv);
	
	if(PI_flags[AYUDA]) {
        print_usage();
        return 0;
    }
	
	pi_cape_ON();
	initSerial();
	set_imu_interrupt_func(&control);
	
	if(PI_flags[DEBUG_BLUETOOTH]){
		pthread_t  serial_thread;
		pthread_create(&serial_thread, NULL, send_Serial, (void*) NULL);
	}
	
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
	
	switch(get_state()){
		
		case RUNNING:		
			
			if(mpu.phi < -15 || mpu.phi > 15){
				set_state(PAUSED);
				disable_motors();
				break;
			}
			
			//graficando PHI
			
			proporcional = mpu.phi * Kp;
			integral = integral + mpu.phi * Ki;
			diferencial = (mpu.phi - mpu.last_phi) * Kd;
			
			mpu.last_phi = mpu.phi;			
						
			mpu.duty = proporcional + integral + diferencial;
			
			//set_motor(2, -mpu.duty);
			//set_motor(1, mpu.duty);
			
			ledLogic();
		
			break;
		
		case PAUSED:
			
			if(mpu.phi > -45 && mpu.phi < 45){
				set_state(RUNNING);
				
				if(!PI_flags[MOTORES_DESACTIVADOS]){
					enable_motors();
				}
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

void* send_Serial(void* ptr){
	
	const int send_us = 20000; // dsm2 packets come in at 11ms, check faster
	char str[30];
	int fd;
	fd = open(uart1_directoryz, O_RDWR | O_NOCTTY);
	
	while(get_state()!=EXITING){
		
		sprintf(str, "E%f,%f\n", mpu.phi , mpu.duty);		
		write(fd, str, strlen(str));
		usleep(send_us);
	}
	
	close(fd);
	printf("Saliendo Hilo Serial\n");
	
	return 0;
}


