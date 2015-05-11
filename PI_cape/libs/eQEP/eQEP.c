/* eQEP.c
 * 
 * PI_ROBOT - PROYECTO CONTROL AUTOMATICO, I SEMESTRE 2015
 * ESCUELA DE INGENIERIA ELECTRONICA, INSTITUTO TECNOLOGICO DE COSTA RICA
 * 
 * Modifications made by Roy Araya, Mechatronics Engineering Student
 * royaraya16@gmail.com
 * 
 * Based on James Strawson's Robotics Cape code.
 * 
 	
Copyright (c) 2014, James Strawson
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


#include "eQEP/eQEP.h"

#define PWM0_BASE   0x48300000
#define PWM1_BASE   0x48302000
#define PWM2_BASE   0x48304000
#define EQEP_OFFSET  0x180

// eQEP Encoder mmap arrays
volatile char *pwm_map_base[3];

int init_eQEP(){

	// mmap pwm modules to get fast access to eQep encoder position
	// see mmap_eqep example program for more mmap and encoder info
	printf("Initializing eQep Encoders\n");
	int dev_mem;
	if ((dev_mem = open("/dev/mem", O_RDWR | O_SYNC))==-1){
	  printf("Could not open /dev/mem \n");
	  return -1;
	}
	pwm_map_base[0] = mmap(0,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,dev_mem,PWM0_BASE);
	pwm_map_base[1] = mmap(0,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,dev_mem,PWM1_BASE);
	pwm_map_base[2] = mmap(0,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,dev_mem,PWM2_BASE);
	if(pwm_map_base[0] == (void *) -1) {
		printf("Unable to mmap pwm \n");
		return(-1);
	}
	close(dev_mem);
	
	// Test eqep and reset position
	int i = 0;
	for(i=1;i<4;i++){
		if(set_encoder_pos(i,0)){
			printf("failed to access eQep register\n");
			printf("eQep driver not loaded\n");
			return -1;
		}
	}
	
	return 0;
}	
	
//// eQep Encoder read/write
long int get_encoder_pos(int ch){
	if(ch<1 || ch>3){
		printf("Encoder Channel must be in 1 or 2\n");
		return -1;
	}
	return  *(unsigned long*)(pwm_map_base[ch-1] + EQEP_OFFSET +QPOSCNT);
}

int set_encoder_pos(int ch, long value){
	if(ch<1 || ch>3){
		printf("Encoder Channel must be 1 or 2\n");
		return -1;
	}
	*(unsigned long*)(pwm_map_base[ch-1] + EQEP_OFFSET +QPOSCNT) = value;
	return 0;
}

