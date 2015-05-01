
#include "Serial/Serial.h"
#include "IMU/IMU.h"

FILE *uart_pointer;
char uart1_directory[30] = "/dev/ttyO1";
struct termios uart1,old;
int fd;

int count = 0;

int initSerial(){
	fd = open(uart1_directory, O_RDWR | O_NOCTTY);
	if(fd < 0) printf("port failed to open\n");

	//save current attributes
	tcgetattr(fd,&old);
	bzero(&uart1,sizeof(uart1)); 

	uart1.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	uart1.c_iflag = IGNPAR | ICRNL;
	uart1.c_oflag = 0;
	uart1.c_lflag = 0;

	uart1.c_cc[VTIME] = 0;
	uart1.c_cc[VMIN]  = 1;

	//clean the line and set the attributes
	tcflush(fd,TCIFLUSH);
	tcsetattr(fd,TCSANOW,&uart1);
	close(fd);
	
	return 0;
}

int SerialWrite(char message[30]){

	fd = open(uart1_directory, O_RDWR | O_NOCTTY);
	write(fd, message, strlen(message));
	close(fd);

	return 0;
}

int debugSerial(float phi, float duty){
	
	char str[30];
	if(count == 4){
		sprintf(str, "E%f,%f\n", phi, duty);
		SerialWrite(str);
		count = 0;
	}
	
	count++;
	
	return 0;
}

void* sendSerial(void *ptr){
	
	const int send_us = 10000; // dsm2 packets come in at 11ms, check faster
	//char str[30];
	//int fd;
	fd = open(uart1_directory, O_WRONLY);
	
	while(get_state()!=EXITING){
		
		//Problema con variable compartida entre hilos
		
		//sprintf(str, "E%f,%f\n", mpu.phi, mpu.duty);
		//SerialWrite(str);
			
		usleep(send_us);
	}
	
	return 0;
}

void* readSerialControl(void *ptr){
	const int check_us = 5000; // dsm2 packets come in at 11ms, check faster
	
	
	while(get_state()!=EXITING){
		
		//leer la vara serial y modificar la referencia o las constantes de Control
		
		// wait for the next frame
		usleep(check_us); 
	}
	return 0;
}
