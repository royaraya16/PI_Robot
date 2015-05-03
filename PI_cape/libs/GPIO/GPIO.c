
//Codigo para exportar los GPIO y otras cosas concernientes a los GPIO

#include "GPIO/GPIO.h"

#define NUM_OUT_PINS 11

unsigned int out_gpio_pins[] = 
					{MDIR1A, MDIR1B, MDIR2A, MDIR2B, 
					 MOT_STBY, GRN_LED, RED_LED, LED_1,
					 LED_2, LED_3, LED_4};

unsigned int led_array[] = 
					{LED_1, LED_2, LED_3, LED_4};
					
int ledCount = 0;
int countSign = 1;
					 
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

int turnOn_leds(){
	int i;
	for(i=0; i<4; i++){
		gpio_set_value(led_array[i], HIGH);
	}
	
	return 0;
}

int turnOff_leds(){
	int i;
	for(i=0; i<4; i++){
		gpio_set_value(led_array[i], LOW);
	}
	
	return 0;
}

int ledLogic(){
	
	turnOff_leds();
	
	gpio_set_value(led_array[ledCount], HIGH);
	
	ledCount = ledCount + countSign;
	
	if(ledCount == 3){
		countSign = -1;
	}
	
	if(ledCount == 0){
		countSign = 1;
	}
	
	return 0;
}

int ledsToggle(){
	
	unsigned int value;
	gpio_get_value(LED_1, &value);
	
	if(value == HIGH){
		turnOff_leds();
	}
	
	else{
		turnOn_leds();
	}
	
	return 0;
}

int led_slowLogic(){
	
	static int slow_ledCount = 0;
	static int slow_countSign = 1;
	static int slow_count = 0;
	
	if(slow_count > 20){
		
		turnOff_leds();
		
		gpio_set_value(led_array[slow_ledCount], HIGH);
		
		slow_ledCount = slow_ledCount + slow_countSign;
		
		if(slow_ledCount == 3){
			slow_countSign = -1;
		}
		
		if(slow_ledCount == 0){
			slow_countSign = 1;
		}
		
		slow_count = 0;
	}
	
	slow_count++;
	
	return 0;
}

