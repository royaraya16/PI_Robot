
#include "Serial/Serial.h"
#include "IMU/IMU.h"


struct termios uart1,old;
int fd;

int count = 0;

int initSerial(){
	fd = open(UART_DIR, O_RDWR | O_NOCTTY);
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
