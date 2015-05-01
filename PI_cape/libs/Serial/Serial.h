
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>

int initSerial();
int SerialWrite(char message[]);
int debugSerial(float phi, float duty);

void *readSerialControl(void *ptr);

void *sendSerial(void *ptr);
