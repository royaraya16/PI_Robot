//Header para AttitudeNEW

#ifndef __ATTITUDENEW_H_INCLUDED__
#define __ATTITUDENEW_H_INCLUDED__

class Attitude{
	public:
		void setup();
		void get(float ypr[3]);
		
		bool dmpReady;  // set true if DMP init was successful
		uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
		uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
		uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
		uint16_t fifoCount;     // count of all bytes currently in FIFO
		uint8_t fifoBuffer[64]; // FIFO storage buffer
	};
#endif
