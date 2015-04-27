
#include "Serial/Serial.h"

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

	uart1.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
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

int SerialWrite(char message[15]){

	fd = open(uart1_directory, O_RDWR | O_NOCTTY);
	write(fd, message, strlen(message));
	close(fd);

	return 0;
}

int debugSerial(float phi){
	
	char str[15];
	if(count == 4){
		sprintf(str, "E%3.2f\n", phi);
		SerialWrite(str);
		count = 0;
	}
	
	count++;
	
	return 0;
}
