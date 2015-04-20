
//eQUEP
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>	// mmap for accessing eQep
#include "eQUEP/tipwmss.h"

#define PWM0_BASE   0x48300000
#define PWM1_BASE   0x48302000
#define PWM2_BASE   0x48304000
#define EQEP_OFFSET  0x180

int init_eQUEP();
long int get_encoder_pos(int ch);
int set_encoder_pos(int ch, long value);
