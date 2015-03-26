

#include "pwm.h"



// pwm stuff
// motors 1-2 driven by pwm 1A, 2A respectively

char pwm_files[][64] = {"/sys/devices/ocp.3/pwm_test_P9_14.12/",
							 "/sys/devices/ocp.3/pwm_test_P8_19.14/"
};

FILE *pwm_duty_pointers[2]; //store pointers to 2 pwm channels for frequent writes
int pwm_period_ns=0; //stores current pwm period in nanoseconds


int initialize_pwm(){
	
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
	
	gpio_set_value((motor-1)*2,a);
	gpio_set_value((motor-1)*2+1,b);
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

