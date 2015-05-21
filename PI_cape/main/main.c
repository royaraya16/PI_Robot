/* PI_ROBOT.c
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Special Thanks to James Strawson and Derek Molloy
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

#define PID_FILE	"PID.txt"

//FILE *PID;

int control();

int main(int argc, char *argv[]){
	
	parse_args(argc, argv);
	time(&mpu.sec);
	
	if(PI_flags[AYUDA]) {
        print_usage();
        return 0;
    }
    
   // PID = fopen(PID_FILE, "w");
	
	
  
	pi_cape_ON();

	setPID(0.069800, 0.007080, 0.493500);
	setEncoder_PID(0.00807, 0.00000395, 0.015575);
	
	set_imu_interrupt_func(&control);
	
	
    while (get_state() != EXITING){	
		//fprintf(PID, "%f\n%f\n%f\n",  robot.Kp, robot.Ki, robot.Kd); 
		usleep(100000); //wait for 1 sec
    }
    
    //fclose(PID);
	
	pi_cape_OFF();
	
	return 0;
}

int control(){
	
	if (mpu6050_read_dmp(&mpu) != 0){
		printf("Error leyendo DMP\n");
		return -1;
	}
	
	robot.encoder = -1* get_encoder_pos(2);
	
	//printf("motor: %ld\n", get_encoder_pos(3)); encoder motor derecho

	
	switch(get_state()){
		
		case RUNNING:		
			
			//Se pregunta si el robot esta apunto de caerse para desconectar motores
			if(mpu.phi < -45 || mpu.phi > 45){
				set_state(PAUSED);
				reset_PID();
				disable_motors();
				turnOff_leds();
				break;
			}
			
			//Control de posicion
			
			robot.proporcional_encoder = robot.encoder * robot.Kp_encoder;
			robot.integral_encoder = robot.integral_encoder + robot.encoder * robot.Ki_encoder;
			robot.diferencial_encoder = (robot.encoder - robot.last_encoder) * robot.Kd_encoder;
			
			robot.last_encoder = robot.encoder;
			
			robot.reference = robot.proporcional_encoder + robot.integral_encoder + robot.diferencial_encoder;
			
			//Control de angulo		
			
			robot.error = mpu.phi - robot.reference;
			
			robot.proporcional = robot.error * robot.Kp;
			robot.integral = robot.integral + robot.error * robot.Ki;
			robot.diferencial = (robot.error - robot.last_error) * robot.Kd;
			
			//Se actualiza el ultimo valor del error para el termino diferencial
			robot.last_error = robot.error;
			
			//Actualizando el valor del ciclo de trabajo de los motores con el PID	
			robot.duty = robot.proporcional + robot.integral + robot.diferencial;
			
			//Mandando las señales a los motores
			set_motor(1, -robot.duty);
			set_motor(2, robot.duty);
		
			break;
		
		case PAUSED:
			
			if(mpu.phi > -1 && mpu.phi < 1){
				set_state(RUNNING);
				mpu.last_phi = mpu.phi;
				set_encoder_pos(2, 0);
				
				if(!PI_flags[MOTORES_DESACTIVADOS]){
					enable_motors();
				}
			}
			break;
			
		case UNINITIALIZED:
		
			time(&mpu.current_time);
			led_slowLogic();
			
			if(difftime(mpu.current_time, mpu.sec) > 20){
				set_state(PAUSED);
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


