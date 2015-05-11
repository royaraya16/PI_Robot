/* PWM.c
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Modifications made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Based on James Strawson's Robotics Cape code.
 * 
 	
Copyright (c) 2014, James Strawson
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

#include "PWM.h"

// pwm stuff
// motors 1-2 driven by pwm 1A, 2A respectively

//REVISAR ESTOS DIRECTORIOS SI SALE SEGMENTATION FAULT

char pwm_files[][64] = {"/sys/devices/ocp.3/pwm_test_P9_14.12/",
							 "/sys/devices/ocp.3/pwm_test_P8_19.13/"
};

FILE *pwm_duty_pointers[2]; //store pointers to 2 pwm channels for frequent writes
int pwm_period_ns=0; //stores current pwm period in nanoseconds

extern unsigned int out_gpio_pins[];

int init_PWM(){
	
	//Que hace initialize?
	//1- Ajusta la polaridad de los PWM
	//2- Abre los archivos de duty y los apunta a los punteros
	//3- Lee el periodo que tienen los pwm y lo guarda en una variable global

	char path[128]; // buffer to store file path string
	FILE *fd; 			// opened and closed for each file
	
	//Ajustando Polaridad
	
	printf("Initializing PWM\n");
	int i=0;
	for(i=0; i<2; i++){
		strcpy(path, pwm_files[i]);
		strcat(path, "polarity");
		fd = fopen(path, "a");
		if(fd<0){
			printf("PWM polarity not available in /sys/class/devices/ocp.3\n");
			return -1;
		}
		//set correct polarity such that 'duty' is time spent HIGH
		fprintf(fd,"%c",'0');
		fflush(fd);
		fclose(fd);
	}
	
	//Abriendo los archivos de duty
	//No importa que se dejen abiertos, el sistema operativo los cierra cuando
	//el programa se termina
	
	//leave duty cycle file open for future writes
	for(i=0; i<2; i++){
		strcpy(path, pwm_files[i]);
		strcat(path, "duty");
		pwm_duty_pointers[i] = fopen(path, "a");
	}
	
	//Leyendo el periodo en nanosegundos y guardando en variable global
	
	//read in the pwm period defined in device tree overlay .dts
	strcpy(path, pwm_files[0]);
	strcat(path, "period");
	fd = fopen(path, "r");
	if(fd<0){
		printf("PWM period not available in /sys/class/devices/ocp.3\n");
		return -1;
	}
	fscanf(fd,"%i", &pwm_period_ns);
	fclose(fd);
	
	return 0;
}


int set_motor(int motor, float duty){
	
	PIN_VALUE a;
	PIN_VALUE b;
	if(motor>2 || motor<1){
		printf("enter a motor value between 1 and 2\n");
		return -1;
	}
	//check that the duty cycle is within +-1
	if (duty>1){
		duty = 1;
	}
	else if(duty<-1){
		duty=-1;
	}
	//switch the direction pins to H-bridge
	if (duty>=0){
	 	a=HIGH;
		b=LOW;
	}
	else{
		a=LOW;
		b=HIGH;
		duty=-duty;
	}
	
	if (motor==1){
		gpio_set_value(MDIR1A,a);
		gpio_set_value(MDIR1B,b);
	}
	else{
		gpio_set_value(MDIR2A,a);
		gpio_set_value(MDIR2B,b);
	}
	
	fprintf(pwm_duty_pointers[motor-1], "%d", (int)(duty*pwm_period_ns));	
	fflush(pwm_duty_pointers[motor-1]);
	return 0;
}

int kill_pwm(){
	//Que hace kill_pwm?
	//Pone todos los valores de pwm en 0, para que no brinque cuando se encienda
	
	int ch;
	if(pwm_duty_pointers[0] == NULL){
		printf("opening pwm duty files\n");
		char path[128];
		int i = 0;
		for(i=0; i<2; i++){
			strcpy(path, pwm_files[i]);
			strcat(path, "duty");
			pwm_duty_pointers[i] = fopen(path, "a");
		}
		printf("opened pwm duty files\n");
	}
	for(ch=0;ch<2;ch++){
		fprintf(pwm_duty_pointers[ch], "%d", 0);	
		fflush(pwm_duty_pointers[ch]);
	}
	return 0;
}

int enable_motors(){
	kill_pwm();
	return gpio_set_value(MOT_STBY, HIGH);
}

int disable_motors(){
	kill_pwm();
	return gpio_set_value(MOT_STBY, LOW);
}

