
//Codigo para exportar los GPIO y otras cosas concernientes a los GPIO

#include "GPIO/GPIO.h"

#define NUM_OUT_PINS 11

unsigned int out_gpio_pins[] = 
					{MDIR1A, MDIR1B, MDIR2A, MDIR2B, 
					 MOT_STBY, GRN_LED, RED_LED, LED_1,
					 LED_2, LED_3, LED_4};

int init_GPIO(){
	
	printf("Initializing GPIO\n");
	
	int i = 0;
	
	for(i=0; i<NUM_OUT_PINS; i++){
		if(gpio_export(out_gpio_pins[i])){
			printf("failed to export gpio %d", out_gpio_pins[i]);
			return -1;
		};
		gpio_set_dir(out_gpio_pins[i], OUTPUT_PIN);
	}
	
	// Desactivar el puente H, por si las moscas
	
	//gpio_set_value(MOT_STBY, LOW);
	
	return 0;
}


