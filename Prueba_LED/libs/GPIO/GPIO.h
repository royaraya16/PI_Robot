
#include <stdio.h>
#include "GPIO/SimpleGPIO.h"

#define RED_LED   66	//gpio2.2	P8.7
#define GRN_LED   67	//gpio2.3	P8.8
#define MDIR2B    79	//gpio2.15  P8.38

#define MOT_STBY  20	//gpio0.20  P9.41
#define MDIR1A    60	//gpio1.28  P9.12
#define MDIR1B    31	//gpio0.31	P9.13
#define MDIR2A    48	//gpio1.16  P9.15

int init_GPIO();
