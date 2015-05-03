
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>

#define UART_DIR "/dev/ttyO1"

int initSerial();

void *readSerialControl(void *ptr);
