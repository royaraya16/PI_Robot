//Codigo para probar los GPIO

#include "Pi_cape/Pi_cape.h"
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol

char *p;
float num;

//Que falta?
//PWM - eQUEP - ADC - i2c - IMU

int main(int argc, char *argv[]){
	
	pi_cape_ON();
	
	pi_cape_OFF();

	return 0;
}
