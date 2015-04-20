
//eQUEP

#include "eQEP/eQEP.h"

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
	//pwm_map_base[2] = mmap(0,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,dev_mem,PWM2_BASE);
	if(pwm_map_base[0] == (void *) -1) {
		printf("Unable to mmap pwm \n");
		return(-1);
	}
	close(dev_mem);
	
	// Test eqep and reset position
	int i = 0;
	for(i=1;i<2;i++){
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
	if(ch<1 || ch>2){
		printf("Encoder Channel must be in 1 or 2\n");
		return -1;
	}
	return  *(unsigned long*)(pwm_map_base[ch-1] + EQEP_OFFSET +QPOSCNT);
}

int set_encoder_pos(int ch, long value){
	if(ch<1 || ch>2){
		printf("Encoder Channel must be 1 or 2\n");
		return -1;
	}
	*(unsigned long*)(pwm_map_base[ch-1] + EQEP_OFFSET +QPOSCNT) = value;
	return 0;
}

