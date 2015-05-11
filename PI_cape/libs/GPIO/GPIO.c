/* GPIO.c
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Special Thanks to James Strawson and Derek Molloy
 * 
 	
Copyright (c) 2015, Roy Araya
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

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

int batteryLed_logic(float voltage){
	
	if(voltage > 8.1){
		gpio_set_value(LED_1, HIGH);
		gpio_set_value(LED_2, HIGH);
		gpio_set_value(LED_3, HIGH);
		gpio_set_value(LED_4, HIGH);
	}
	
	else if(voltage > 7.8){
		gpio_set_value(LED_1, HIGH);
		gpio_set_value(LED_2, HIGH);
		gpio_set_value(LED_3, HIGH);
		gpio_set_value(LED_4, LOW);
	}
	
	else if(voltage > 7.5){
		gpio_set_value(LED_1, HIGH);
		gpio_set_value(LED_2, HIGH);
		gpio_set_value(LED_3, LOW);
		gpio_set_value(LED_4, LOW);
	}
	
	else if(voltage > 7.4){
		gpio_set_value(LED_1, HIGH);
		gpio_set_value(LED_2, LOW);
		gpio_set_value(LED_3, LOW);
		gpio_set_value(LED_4, LOW);
	}
	
	return 0;
}

