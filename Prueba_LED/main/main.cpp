//Codigo para probar los GPIO

#include <iostream>
#include <string>
#include <unistd.h>
#include "GPIO/SimpleGPIO.h"

using namespace std;

#define RED_LED 66

int main(int argc, char *argv[]){

	if(argc > 1){
		cout << "Hola Mundo" << endl;
	}

	gpio_export(RED_LED);
	gpio_set_dir(RED_LED, OUTPUT_PIN);
	
	if(argc>1){
		gpio_set_value(RED_LED, LOW);
	}
	else{
		gpio_set_value(RED_LED, HIGH);
	}

	return 0;
}
