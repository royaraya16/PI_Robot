
#include "ADC/ADC.h"

float getBattVoltage(){
	int raw_adc;
	FILE *AIN6_fd;
	AIN6_fd = fopen("/sys/devices/ocp.3/helper.14/AIN6", "r");
	
	if(AIN6_fd < 0){
		printf("error reading adc\n");
		return -1;
	}
	
	fscanf(AIN6_fd, "%i", &raw_adc);
	fclose(AIN6_fd);
	// times 11 for the voltage divider, divide by 1000 to go from mv to V
	return (float)raw_adc*11.0/1000.0; 
}
