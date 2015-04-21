
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
	init_IMU();
	
	//inicializar interrupts
	//init_interrupts();
	
	//Imprimir el estado actual de la bateria
	//printf("Tensión Batería = %3.2f Volts\n", getBattVoltage());
	
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
	 
	//setGRN(0);
	//setRED(0);	
	disable_motors();
	
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
