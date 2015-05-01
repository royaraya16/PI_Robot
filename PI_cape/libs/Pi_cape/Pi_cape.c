
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
	
	//inicializar interrupts
	//init_interrupts();
	
	//condicion inicial para el ultimo valor de phi
	mpu.last_phi = 0;
	
	//Imprimir el estado actual de la bateria
	printf("Tensión Batería = %3.2f Volts\n", getBattVoltage());
	
	/*if(getBattVoltage() < 7.5){
		printf("---BATERIA BAJA!!!---\n");
		set_state(EXITING);
		return -1;
	}*/
	
	//Desactivar motores por si las moscas
	disable_motors();
	
	// Start Signal Handler
	printf("Enabling exit signal handler\n");
	signal(SIGINT, ctrl_c);	
	
	//Estableciendo el estado en pausado, esperando que se posicione verticalmente
	set_state(PAUSED);
	
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
