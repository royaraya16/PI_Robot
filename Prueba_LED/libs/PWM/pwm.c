


//// eQep and pwmss registers, more in tipwmss.h
#define PWM0_BASE   0x48300000
#define PWM1_BASE   0x48302000
#define PWM2_BASE   0x48304000
#define EQEP_OFFSET  0x180

// pwm stuff
// motors 1-4 driven by pwm 1A, 1B, 2A, 2B respectively
char pwm_files[][64] = {"/sys/devices/ocp.3/pwm_test_P9_14.12/",
							 "/sys/devices/ocp.3/pwm_test_P9_16.13/",
							 "/sys/devices/ocp.3/pwm_test_P8_19.14/",
							 "/sys/devices/ocp.3/pwm_test_P8_13.15/"
};
FILE *pwm_duty_pointers[4]; //store pointers to 4 pwm channels for frequent writes
int pwm_period_ns=0; //stores current pwm period in nanoseconds


// eQEP Encoder mmap arrays
volatile char *pwm_map_base[3];

int initialize_pwm(){

//Set up PWM
	printf("Initializing PWM\n");
	i=0;
	for(i=0; i<4; i++){
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
	
	//leave duty cycle file open for future writes
	for(i=0; i<4; i++){
		strcpy(path, pwm_files[i]);
		strcat(path, "duty");
		pwm_duty_pointers[i] = fopen(path, "a");
	}
	
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
	if(state == UNINITIALIZED){
		initialize_cape();
	}
	if(motor>4 || motor<1){
		printf("enter a motor value between 1 and 4\n");
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
	gpio_set_value(out_gpio_pins[(motor-1)*2],a);
	gpio_set_value(out_gpio_pins[(motor-1)*2+1],b);
	fprintf(pwm_duty_pointers[motor-1], "%d", (int)(duty*pwm_period_ns));	
	fflush(pwm_duty_pointers[motor-1]);
	return 0;
}

int kill_pwm(){
	int ch;
	if(pwm_duty_pointers[0] == NULL){
		printf("opening pwm duty files\n");
		char path[128];
		int i = 0;
		for(i=0; i<4; i++){
			strcpy(path, pwm_files[i]);
			strcat(path, "duty");
			pwm_duty_pointers[i] = fopen(path, "a");
		}
		printf("opened pwm duty files\n");
	}
	for(ch=0;ch<4;ch++){
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

