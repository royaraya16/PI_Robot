
#include "Pi_cape/Pi_cape.h"


int pi_cape_ON(){
	
	//Chequear que no este corriendo el programa otra vez
	//checkProcess();
	
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
	mpu.phi = 0;
	robot.error = 0;
	mpu.last_euler[0] = 99.9;
	mpu.last_euler[1] = 99.9;
	mpu.last_euler[2] = 99.9;
	
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
	printf("Enabling exit signal handler\n");
	signal(SIGINT, ctrl_c);	
	
	//Estableciendo el estado en inicializado, esperando que el sensor se estabilice
	set_state(UNINITIALIZED);
	
	printf("---------\nPI_ROBOT ON\n---------\n");
	
	return 0;
}

int pi_cape_OFF(){
	set_state(EXITING);
	usleep(500000); // let final threads clean up
	FILE* fd;
	// clean up the lockfile if it still exists
	fd = fopen(LOCKFILE, "r");
	if (fd != NULL) {
		// close and delete the old file
		fclose(fd);
		remove(LOCKFILE);
	}
	
	disable_motors();
	turnOff_leds();
	
	printf("---------\nPI_ROBOT OFF\n---------\n");
	
	return 0;	
}

int checkProcess(){	
	FILE *fd; 			// opened and closed for each file
	fd = fopen(LOCKFILE, "r");
	if (fd != NULL) {
		int old_pid;
		fscanf(fd,"%d", &old_pid);
		if(old_pid != 0){
			printf("warning, shutting down existing robotics project\n");
			kill((pid_t)old_pid, SIGINT);
			sleep(1);
		}
		// close and delete the old file
		fclose(fd);
		remove(LOCKFILE);
	}	
	
	// create new lock file with process id
	fd = fopen(LOCKFILE, "ab+");
	if (fd < 0) {
		printf("\n error opening LOCKFILE for writing\n");
		return -1;
	}
	pid_t current_pid = getpid();
	printf("Current Process ID: %d\n", (int)current_pid);
	fprintf(fd,"%d",(int)current_pid);
	fflush(fd);
	fclose(fd);
	
	return 0;	
}

void ctrl_c(int signo){
	if (signo == SIGINT){
		set_state(EXITING);
		printf("\nreceived SIGINT Ctrl-C\n");
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

int reset_PID(){
	robot.integral = 0;
	
	return 0;
}

float map(float x, float in_min, float in_max, float out_min, float out_max){
	
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}


void* send_Serial(void* ptr){
	
	const int send_us = 40000; // enviar datos cada 40ms, si se mandan muy rapido la aplicacion se cae
	char str[30];
	int fd;
	fd = open(UART_DIR, O_RDWR | O_NOCTTY);
	
	while(get_state()!=EXITING){
		
		sprintf(str, "E%f,%f\n", mpu.phi , robot.duty);		
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
			
			
			usleep(monitor_us);
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
	}
	
	printf("Saliendo Hilo Bateria\n");
	
	return 0;
}

void* readSerialControl(void *ptr){
	
	const int check_us = 100000; // dsm2 packets come in at 11ms, check faster
	int ttyO1_fd, res, i;
	int state = WAITING;
	int tempCount = 0;
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
							break;						
						case SP:
							//printf("termino p = %f\n", atof(tmp));
							robot.Kp = map(atof(tmp), 0, 20, 0, 1);
							break;
						case SI:
							//printf("termino i = %f\n", atof(tmp));
							robot.Ki = map(atof(tmp), 0, 20, 0, 0.02);
							break;
						case SD:
							//printf("termino d = %f\n", atof(tmp));
							robot.Kd = map(atof(tmp), 0, 20, 0, 1);
							break;
						case ST:
							//printf("referencia = %f\n", atof(tmp));
							robot.reference = map(atof(tmp), 150, 210, -2, 2);
							break;
						default:
							break;
					}
					
					state = WAITING;
					//printf("%s\n", tmp);
					memset(&tmp[0], 0, sizeof(tmp));
					tempCount = 0;
				}
				
				else if(robot.buffer[i] == sep2){ // caracteres SP, SI, SD o ST
					//printf("%s\n", tmp);
					
					if(!strcmp(tmp, "SP"))
						state = SP;
						
					else if(!strcmp(tmp, "SI"))
						state = SI;
						
					else if(!strcmp(tmp, "SD"))
						state = SD;
						
					else if(!strcmp(tmp, "SP"))
						state = SP;
					
					else if(!strcmp(tmp, "ST"))
						state = ST;
						
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
