//Codigo para probar los GPIO

#include "Pi_cape/Pi_cape.h"
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>  // for strtol

char *p;
float num;

//variables del IMU
static float last_euler[3] = { 99.9, 99.9, 99.9 };
static float quat_offset[4] = { 0.0F };

//Que falta?
//PWM - eQUEP - ADC - i2c - IMU

int main(int argc, char *argv[]){
	
	pi_cape_ON();
	
	//Codigo para poder leer el IMU con interrupts---------------------------
	init_IMU();
    short accel[3], gyro[3], sensors[1];
    long quat[4];
    unsigned long timestamp;
    unsigned char more[0];
    struct pollfd fdset[1];
    char buf[1];

    // File descriptor for the GPIO interrupt pin
    int gpio_fd = open(GPIO_INT_FILE, O_RDONLY | O_NONBLOCK);

    // Create an event on the GPIO value file
    memset((void*)fdset, 0, sizeof(fdset));
    fdset[0].fd = gpio_fd;
    fdset[0].events = POLLPRI;
    time_t sec, current_time; // set to the time before calibration

    time(&sec);

    while (1){
		
        poll(fdset, 1, -1);        
            
        if (fdset[0].revents & POLLPRI) {
            // Read the file to make it reset the interrupt
			read(fdset[0].fd, buf, 1);

            int fifo_read = dmp_read_fifo(gyro, accel, quat, &timestamp, sensors, more);
            if (fifo_read != 0) {
                //printf("Error reading fifo.\n");
                continue;
            }

            float angles[13];
            rescale_l(quat, angles+9, QUAT_SCALE, 4);
            
			if (!quat_offset[0]) {
                advance_spinner(); // Heartbeat to let the user know we are running"
                euler(angles+9, angles); // Determine calibration based on settled Euler angles
                // check if the IMU has finished calibrating
                time(&current_time);
                // check if more than CALIBRATION_TIME seconds has passed since calibration started
                if((fabs(last_euler[0]-angles[0]) < THRESHOLD
                        && fabs(last_euler[1]-angles[1]) < THRESHOLD
                        && fabs(last_euler[2]-angles[2]) < THRESHOLD)
                        || difftime(current_time, sec) > CALIBRATION_TIME) {

					printf("\nCALIBRATED! Threshold: %f Elapsed time: %f\n", CALIBRATION_TIME, difftime(current_time, sec));
					printf("CALIBRATED! Threshold: %.5f Errors: %.5f %.5f %.5f\n", THRESHOLD, fabs(last_euler[0]-angles[0]), last_euler[1]-angles[1], last_euler[2]-angles[2]);
				    
                    // the IMU has finished calibrating
                    int i;
                    quat_offset[0] = angles[9]; // treat the w value separately as it does not need to be reversed
                    for(i=1;i<4;++i){
                        quat_offset[i] = -angles[i+9];
                    }
                }
                else {
                    memcpy(last_euler, angles, 3*sizeof(float));
                }
            }
            else {
                q_multiply(quat_offset, angles+9, angles+9); // multiply the current quaternstion by the offset caputured above to re-zero the values
                // rescale the gyro and accel values received from the IMU from longs that the
                // it uses for efficiency to the floats that they actually are and store these values in the angles array
                rescale_s(gyro, angles+3, GYRO_SCALE, 3);
                rescale_s(accel, angles+6, ACCEL_SCALE, 3);
                // turn the quaternation (that is already in angles) into euler angles and store it in the angles array
                euler(angles+9, angles);

				printf("Yaw: %+5.1f\tPitch: %+5.1f\tRoll: %+5.1f\n", angles[0]*180.0/PI, angles[1]*180.0/PI, angles[2]*180.0/PI);
                
            }
        }
    }
	//Aqui termina el codigo del IMU----------------------------------------------------
	
	pi_cape_OFF();

	return 0;
}
