//Codigo para probar los GPIO

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "GPIO/GPIO.h"
#include "PWM/pwm.h"
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol

char *p;
float num;

//Que falta?
//PWM - eQUEP - ADC - i2c - IMU

int main(int argc, char *argv[]){

	init_GPIO();
	init_PWM();
	
	if(argc > 1){
		errno = 0;
		long conv = strtol(argv[1], &p, 10);

		// Check for errors: e.g., the string does not represent an integer
		// or the integer is larger than int
		
		if (errno != 0 || *p != '\0' || conv > INT_MAX) {
			// Put here the handling of the error, like exiting the program with
			// an error message
			printf("Debe poner un numero\n");
			return -1;
		} else {
			// No error
			if(conv > 100 || conv < 0){
				printf("El num debe estar entre 0 y 100\n");
				return -1;
			}
			
			num = (float)conv/100.0;
			printf("PWM = %3.2f\n", num);
		}
	}
	
	set_motor(1, num);

	return 0;
}
