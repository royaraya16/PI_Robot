#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "IMU.h"

Attitude IMU;

int main() {
	
	IMU.setup();
	usleep(100000);
	
	float ypr[3];           // [yaw, pitch, roll]
	        
    while(1){
		IMU.get(ypr);
		printf("Yaw: %7.2f Pitch: %7.2f Roll: %7.2f", ypr[0], ypr[1], ypr[2]);
		printf("\n");
		usleep(1000);
	}	
    return 0;
}
	
