
//interrupts

#include "interrupts/interrupts.h"

enum state_t state = UNINITIALIZED;

enum state_t get_state(){
	return state;
}

int set_state(enum state_t new_state){
	state = new_state;
	return 0;
}

// buttons
int pause_btn_state, mode_btn_state;
int (*imu_interrupt_func)();
int (*pause_unpressed_func)();
int (*pause_pressed_func)();
int (*mode_unpressed_func)();
int (*mode_pressed_func)();

//function pointers for events initialized to null_func()
//instead of containing a null pointer
int null_func(){
	return 0;
}

int set_pause_pressed_func(int (*func)(void)){
	pause_pressed_func = func;
	return 0;
}
int set_pause_unpressed_func(int (*func)(void)){
	pause_unpressed_func = func;
	return 0;
}
int set_mode_pressed_func(int (*func)(void)){
	mode_pressed_func = func;
	return 0;
}
int set_mode_unpressed_func(int (*func)(void)){
	mode_unpressed_func = func;
	return 0;
}

int get_pause_button_state(){
	return pause_btn_state;
}

int get_mode_button_state(){
	return mode_btn_state;
}

int init_buttons(){
	//set up function pointers for button press events
	printf("starting button interrupts\n");
	set_pause_pressed_func(&null_func);
	set_pause_unpressed_func(&null_func);
	set_mode_pressed_func(&null_func);
	set_mode_unpressed_func(&null_func);
	initialize_button_handlers();
	
	return 0;
}

/***********************************************************************
*	int initialize_button_interrups()
*	start 4 threads to handle 4 interrupt routines for pressing and
*	releasing the two buttons.
************************************************************************/
int initialize_button_handlers(){
	pthread_t pause_pressed_thread;
	pthread_t pause_unpressed_thread;
	pthread_t mode_pressed_thread;
	pthread_t mode_unpressed_thread;
	struct sched_param params;
	
	params.sched_priority = sched_get_priority_max(SCHED_FIFO)/2;
	
	pthread_create(&pause_pressed_thread, NULL,			 \
				pause_pressed_handler, (void*) NULL);
	pthread_create(&pause_unpressed_thread, NULL,			 \
				pause_unpressed_handler, (void*) NULL);
	pthread_create(&mode_pressed_thread, NULL,			 \
					mode_pressed_handler, (void*) NULL);
	pthread_create(&mode_unpressed_thread, NULL,			 \
					mode_unpressed_handler, (void*) NULL);
	
	// apply medium priority to all threads
	pthread_setschedparam(pause_pressed_thread, SCHED_FIFO, &params);
	pthread_setschedparam(pause_unpressed_thread, SCHED_FIFO, &params);
	pthread_setschedparam(mode_pressed_thread, SCHED_FIFO, &params);
	pthread_setschedparam(mode_unpressed_thread, SCHED_FIFO, &params);
	 
	return 0;
}

/***********************************************************************
*	void* pause_pressed_handler(void* ptr) 
*	wait on falling edge of pause button
************************************************************************/
void* pause_pressed_handler(void* ptr){
	int fd;
    fd = open("/dev/input/event1", O_RDONLY);
    struct input_event ev;
	while (get_state() != EXITING){
        read(fd, &ev, sizeof(struct input_event));
		// uncomment printf to see how event codes work
		// printf("type %i key %i state %i\n", ev.type, ev.code, ev.value); 
        if(ev.type==1 && ev.code==1){ //only new data
			if(ev.value == 1){
				pause_btn_state = PRESSED;
				(*pause_pressed_func)();
			}
		}
		usleep(10000); // wait
    }
	return NULL;
}

/***********************************************************************
*	void* pause_unpressed_handler(void* ptr) 
*	wait on rising edge of pause button
************************************************************************/
void* pause_unpressed_handler(void* ptr){
	int fd;
    fd = open("/dev/input/event1", O_RDONLY);
    struct input_event ev;
	while (get_state() != EXITING){
        read(fd, &ev, sizeof(struct input_event));
		// uncomment printf to see how event codes work
		// printf("type %i key %i state %i\n", ev.type, ev.code, ev.value); 
        if(ev.type==1 && ev.code==1){ //only new data
			if(ev.value == 0){
			
				pause_btn_state = UNPRESSED;
				(*pause_unpressed_func)();
			}
		}
		usleep(10000); // wait
    }
	return NULL;
}

/***********************************************************************
*	void* mode_pressed_handler(void* ptr) 
*	wait on falling edge of mode button
************************************************************************/
void* mode_pressed_handler(void* ptr){
	int fd;
    fd = open("/dev/input/event1", O_RDONLY);
    struct input_event ev;
	while (get_state() != EXITING){
        read(fd, &ev, sizeof(struct input_event));
		// uncomment printf to see how event codes work
		// printf("type %i key %i state %i\n", ev.type, ev.code, ev.value); 
        if(ev.type==1 && ev.code==2){ //only new data
			if(ev.value == 1){
				mode_btn_state = PRESSED;
				(*mode_pressed_func)();
			}
		}
		usleep(10000); // wait
    }
	return NULL;
}

/***********************************************************************
*	void* mode_unpressed_handler(void* ptr) 
*	wait on rising edge of mode button
************************************************************************/
void* mode_unpressed_handler(void* ptr){
	int fd;
    fd = open("/dev/input/event1", O_RDONLY);
    struct input_event ev;
	while (get_state() != EXITING){
        read(fd, &ev, sizeof(struct input_event));
		// uncomment printf to see how event codes work
		// printf("type %i key %i state %i\n", ev.type, ev.code, ev.value); 
        if(ev.type==1 && ev.code==2){ //only new data
			if(ev.value == 0){
				mode_btn_state = UNPRESSED; //pressed
				(*mode_unpressed_func)();
			}
		}
		usleep(10000); // wait
    }
	return NULL;
}

// IMU interrupt stuff
// see test_imu and calibrate_gyro for examples
int set_imu_interrupt_func(int (*func)(void)){
	imu_interrupt_func = func;
	return 0;
}

void* imu_interrupt_handler(void* ptr){
	struct pollfd fdset[1];
	char buf[MAX_BUF];
	int imu_gpio_fd = gpio_fd_open(INTERRUPT_PIN);
	fdset[0].fd = imu_gpio_fd;
	fdset[0].events = POLLPRI; // high-priority interrupt
	// keep running until the program closes
	while(get_state() != EXITING) {
		// system hangs here until IMU FIFO interrupt
		poll(fdset, 1, POLL_TIMEOUT);        
		if (fdset[0].revents & POLLPRI) {
			lseek(fdset[0].fd, 0, SEEK_SET);  
			read(fdset[0].fd, buf, MAX_BUF);
			// user selectable with set_inu_interrupt_func() defined above
			imu_interrupt_func(); 
		}
	}
	gpio_fd_close(imu_gpio_fd);
	return 0;
}
