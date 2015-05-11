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

int control();

int main(int argc, char *argv[]){
	
	parse_args(argc, argv);
	time(&mpu.sec);
	
	if(PI_flags[AYUDA]) {
        print_usage();
        return 0;
    }
  
	pi_cape_ON();
	//setPID(0.15, 0.005, 0.35); oscila un poco y jala hacia el lado de la bateria
	//setPID(0.2, 0.005, 0.35); oscila un poco mas pero dura mas tiempo de pie
	//con mas P mas oscilacion . con mas I mas velocidad
	//setPID(0.2, 0.01, 0.35); oscila con mas velocidad
	//setPID(0.2, 0.01, 0.30); menos D satura el business
	//setPID(0.2, 0.01, 0.4); con mas D aumenta el equilibriopz
	//setPID(0.15, 0.01, 0.4); muy lindis pero se va para el lado de la bateria
	//setPID(0.13, 0.01, 0.4); se tira para el lado del beagle
	//setPID(0.14, 0.01, 0.4); se va hacia la bateria pero no hay oscilacion
	//setPID(0.135, 0.01, 0.4); se va para el beagle de nuevo ARGH
	//setPID(0.14, 0.02, 0.4); se satura
	//setPID(0.14, 0.01, 0.4); esta raro
	//setPID(0.14, 0.005, 0.4); ya funciona otra vez
	//setPID(0.14, 0.007, 0.45);
	//setPID(0.14, 0.009, 0.5); siempre se tira el cabron
	setPID(0.1195, 0.00126, 0.778);
	
	set_imu_interrupt_func(&control);
	
	
    while (get_state() != EXITING){		
		usleep(100000); //wait for 1 sec
    }
	
	pi_cape_OFF();
	
	return 0;
}

int control(){
	
	if (mpu6050_read_dmp(&mpu) != 0){
		printf("Error leyendo DMP\n");
		return -1;
	}
	
	//printf("motor: %ld\n", get_encoder_pos(3)); encoder motor derecho

	
	switch(get_state()){
		
		case RUNNING:		
			
			//Se pregunta si el robot esta apunto de caerse para desconectar motores
			if(mpu.phi < -15 || mpu.phi > 15){
				set_state(PAUSED);
				reset_PID();
				disable_motors();
				turnOff_leds();
				break;
			}
			
			
			robot.error = mpu.phi - robot.reference;
			
			//Se actualizan los valores del PID, dependiendo del error y las constantes del PID
			
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


