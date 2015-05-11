/* GPIO.h
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

#include <stdio.h>
#include "GPIO/SimpleGPIO.h"

#define RED_LED   66	//gpio2.2	P8.7
#define GRN_LED   67	//gpio2.3	P8.8
#define LED_1	  26	//gpioxxx	P8.14
#define LED_2	  46	//gpioxxx	P8.15
#define LED_3	  47	//gpioxxx	P8.16
#define LED_4	  27	//gpioxxx	P8.17
#define MDIR2B    79	//gpio2.15  P8.38

#define MOT_STBY  20	//gpio0.20  P9.41
#define MDIR1A    115	//gpio1.28  P9.27
#define MDIR1B    49	//gpio0.31	P9.42
#define MDIR2A    7 	//gpio1.16  P9.23

int init_GPIO();
int turnOn_leds();
int turnOff_leds();
int ledLogic();
int ledsToggle();
int led_slowLogic();
int batteryLed_logic(float voltage);
