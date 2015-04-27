
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
int turnOff_leds();
int ledLogic();
