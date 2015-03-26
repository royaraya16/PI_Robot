//Codigo para probar los GPIO

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "GPIO/GPIO.h"

//Que falta?
//PWM - eQUEP - ADC - i2c - IMU

int main(int argc, char *argv[]){

	init_GPIO();
	
	if(argc > 1){
			printf("HOLA ROY Probandobotones\n");
	}
	
	if(argc>1){
		gpio_set_value(RED_LED, LOW);
	}
	else{
		gpio_set_value(RED_LED, HIGH);
	}

	return 0;
}
