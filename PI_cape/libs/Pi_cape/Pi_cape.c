/* Pi_cape.c
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Modifications made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Based on James Strawson's Robotics Cape code.
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

int pi_cape_ON(){
	
	//exportar e inicializar los GPIO
	init_GPIO();
	
	//inicializar los PWM y crear punteros para el ciclo de trabajo
	init_PWM();
	
	//inicializar eQEP
	init_eQEP();
	
	//inicializar IMU
	init_IMU(PI_flags[CALIBRACION]);
	init_IMU_thread();
	
	//inicializar comunicacion serial
	initSerial();
	
	//Funcion para terminar el programa con el boton
	pthread_t button_thread;
	pthread_create(&button_thread, NULL, boton_handler, (void*) NULL);
	
	//Iniciar hilo serial de telemetria si la bandera esta activa
		
	if(PI_flags[DEBUG_BLUETOOTH]){
		pthread_t serial_thread;
		pthread_create(&serial_thread, NULL, send_Serial, (void*) NULL);
	}
	
	if(PI_flags[CONTROL_REMOTO]){
		pthread_t remote_thread;
		pthread_create(&remote_thread, NULL, readSerialControl, (void*) NULL);
	}
	
	//estableciendo condiciones iniciales
	mpu.last_phi = 0;
	robot.last_error = 0;
	robot.integral = 0;
	robot.integral_encoder = 0;
	robot.last_encoder = 0;
	mpu.phi = 0;
	robot.error = 0;
	robot.reference = 0;
	robot.jX = 0;
	robot.jY = 0;
	
	if(PI_flags[CALIBRACION]){
		mpu.calibrated = 0;
	}
	
	else{
		FILE *cal2;
		cal2 = fopen(IMU_CAL_FILE2, "r");
		fscanf(cal2, "%f\n%f\n%f\n%f\n",  &mpu.scaled_quat_offset[0], &mpu.scaled_quat_offset[1], &mpu.scaled_quat_offset[2], &mpu.scaled_quat_offset[3]); 
		fclose(cal2);	
		mpu.calibrated = 1;
	}
	
	robot.voltage = getBattVoltage();
	
	//Imprimir el estado actual de la bateria
	
	if(robot.voltage < 2){
		printf("Conectado por USB\n");
	}
	
	else if(robot.voltage < 6){
		printf("Conectado con DC JACK\n");
	}
	
	else{
		printf("Tensión Batería = %3.2f Volts\n", robot.voltage);
		
		pthread_t battery_thread;
		pthread_create(&battery_thread, NULL, battery_monitor, (void*) NULL);
		
		if(robot.voltage < 7.4){
			printf("---BATERIA BAJA!!!---\n");
			set_state(EXITING);
			return -1;
		}
	}
	//Desactivar motores por si las moscas
	disable_motors();
	
	// Start Signal Handler
	signal(SIGINT, ctrl_c);	
	
	//Estableciendo el estado en UNITIALIZED, esperando que el sensor se estabilice
	set_state(UNINITIALIZED);
	
	printf("---------\nPI_ROBOT ON\n---------\n");
	
	return 0;
}

int pi_cape_OFF(){
	
	set_state(EXITING);
	usleep(900000); // dejando que los hilos terminen
	
	disable_motors();
	turnOff_leds();
	
	printf("---------\nPI_ROBOT OFF\n---------\n");
	
	return 0;	
}

void ctrl_c(int signo){
	if (signo == SIGINT){
		set_state(EXITING);
		printf("\nCtrl-C - Saliendo\n");
 	}
}

void parse_args(int argc, char** argv) {
    int ch;
    
    //flag i for no interrupt, v for no verbose
    while((ch=getopt(argc, argv, "drmch?")) != -1) {
        switch(ch) {
            case 'd': PI_flags[DEBUG_BLUETOOTH]=1; break;
            case 'r': PI_flags[CONTROL_REMOTO]=1; break;
            case 'm': PI_flags[MOTORES_DESACTIVADOS]=1; break;
            case 'c': PI_flags[CALIBRACION]=1; break;
            case 'h':
            case '?': PI_flags[AYUDA]=1; break;
        }
    }
}

void print_usage() {
    printf("\nPI-Robot, proyecto de Control Automatico, ITCR:\n\n");
    printf("Uso: ejecutable [-d] [-r] [-m] [-cal] [-h, -?]\n\n");
    printf("Argumentos:\n");
    printf("-d\tDebug por medio de bluetooth\n");
    printf("-r\tControl remoto con aplicación\n");
    printf("-m\tCon motores desactivados\n");
    printf("-c\tModo de calibración\n"); //El modo de calibración va a guardar los datos obtenidos en un archivo para que no se deba calibrar cada vez que se ejecuta el programa
    printf("-h, -?\tImprimir esto, luego salir\n");
}

int setPID(float Kp, float Ki, float Kd){
	
	robot.Kp = Kp;
	robot.Ki = Ki;
	robot.Kd = Kd;
	
	return 0;
}

int setEncoder_PID(float Kp, float Ki, float Kd){
	
	robot.Kp_encoder = Kp;
	robot.Ki_encoder = Ki;
	robot.Kd_encoder = Kd;
	
	return 0;
}

int reset_PID(){
	robot.integral = 0;
	robot.integral_encoder = 0;
	robot.last_encoder = 0;
	//robot.last_error = 0;
	return 0;
}

//funcion para escalar valores, tipo regla de 3
float map(float x, float in_min, float in_max, float out_min, float out_max){
	
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}


void* send_Serial(void* ptr){
	
	const int send_us = 40000; // enviar datos cada 40ms, si se mandan muy rapido la aplicacion se cae
	char str[30];
	int fd;
	fd = open(UART_DIR, O_RDWR | O_NOCTTY);
	
	while(get_state()!=EXITING){
		
		//sprintf(str, "E%f,%f\n", mpu.phi , robot.duty);
		sprintf(str, "E%3.2f,%3.2f,%3.2f\n", robot.duty, robot.Kd, robot.Ki);			
		write(fd, str, strlen(str));
		usleep(send_us);
	}
	
	close(fd);
	printf("Saliendo Hilo Serial\n");
	
	return 0;
}

void* battery_monitor(void* ptr){
	
	const int monitor_us = 1000000; // monitoreo cada 4s
	
	while(get_state()!=EXITING){
		//cambiar los leds para que se prendan de acuerdo a la tension de la bateria
		
		if(get_state() == RUNNING || get_state() == PAUSED){
			
			if(robot.voltage > 8.1){
				gpio_set_value(LED_1, HIGH);
			}
			
			if(robot.voltage > 7.8){
				gpio_set_value(LED_2, HIGH);
			}
			
			if(robot.voltage > 7.5){
				gpio_set_value(LED_3, HIGH);
			}
			
			if(robot.voltage > 7.4){
				gpio_set_value(LED_4, HIGH);
			}
			
			turnOff_leds();
		}
		
		usleep(monitor_us);		
	}
	
	printf("Saliendo Hilo Bateria\n");
	
	return 0;
}

void* readSerialControl(void *ptr){
	
	const int check_us = 100000; // dsm2 packets come in at 11ms, check faster
	int ttyO1_fd, res, i;
	int state = WAITING;
	int tempCount = 0;
	int ready = 0;
	char tmp[8]={0};
	char sep1 = ';';
	char sep2 = ',';
	
	ttyO1_fd = open(UART_DIR, O_RDWR | O_NOCTTY);
	
	while(get_state()!=EXITING){
		
		//leer la vara serial y modificar la referencia o las constantes de Control
		res = read(ttyO1_fd, robot.buffer, 32);
        if(res){
			for(i = 0; i<res; i++){
							
				if(robot.buffer[i] == sep1){
					
					switch(state){
						case WAITING:						
							robot.jX = 0;
							robot.jY = 0;
							break;						
						case SP:
							//robot.Kp = map(atof(tmp), 0, 20, 0, 0.1);
							robot.Kp_encoder = map(atof(tmp), 0, 20, 0, 0.02);
							break;
						case SI:
							//robot.Ki = map(atof(tmp), 0, 20, 0, 0.01);
							robot.Ki_encoder = map(atof(tmp), 0, 20, 0, 0.0001);
							break;
						case SD:
							//robot.Kd = map(atof(tmp), 0, 20, 0, 1);
							robot.Kd_encoder = map(atof(tmp), 0, 20, 0, 0.1);
							break;
						case JOYSTICK:
							robot.jY = atof(tmp);
							break;
						default:
							break;
					}
					
					state = WAITING;
					memset(&tmp[0], 0, sizeof(tmp));
					tempCount = 0;
				}
				
				else if(robot.buffer[i] == sep2){
					
					if(ready){
						robot.jX = atof(tmp);
						ready = 0;
						state = JOYSTICK;					
					}
					
					if(!strcmp(tmp, "SP"))
						state = SP;
						
					else if(!strcmp(tmp, "SI"))
						state = SI;
						
					else if(!strcmp(tmp, "SD"))
						state = SD;
						
					else if(!strcmp(tmp, "SP"))
						state = SP;
					
					else if(!strcmp(tmp, "CJ")){
						state = JOYSTICK;
						ready = 1;
					}
						
					memset(&tmp[0], 0, sizeof(tmp));
					tempCount = 0;
				}
				
				else{
					tmp[tempCount] = robot.buffer[i];
					tempCount++;
				}
			}
		}
		usleep(check_us); 
	}
	
	close(ttyO1_fd);
	
	return 0;
}

void* boton_handler(void* ptr){
	int fd;
    fd = open("/dev/input/event1", O_RDONLY);
    struct input_event ev;
	while (get_state() != EXITING){
        read(fd, &ev, sizeof(struct input_event));
		// uncomment printf to see how event codes work
		// printf("type %i key %i state %i\n", ev.type, ev.code, ev.value); 
        if(ev.type==1 && ev.code==1){ //only new data
			if(ev.value == 1){
				set_state(EXITING);
			}
		}
		usleep(10000); // wait
    }
    
    printf("Saliendo hilo del boton\n");
	return NULL;
}
