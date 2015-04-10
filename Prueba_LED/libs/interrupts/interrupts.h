
#include "stdio.h"
#include <unistd.h>
#include <signal.h>		// capture ctrl-c
#include <pthread.h>    // multi-threading
#include <linux/input.h>// buttons
#include <poll.h> 		// interrupt events
#include <fcntl.h>
#include <sys/mman.h>	// mmap for accessing eQep
#include "GPIO/SimpleGPIO.h"

#define INTERRUPT_PIN 117  //gpio3.21 P9.25

#define PRESSED 1
#define UNPRESSED 0

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */

#define MAX_BUF 64

typedef enum state_t {
	UNINITIALIZED,
	RUNNING,
	PAUSED,
	EXITING
} state_t;

enum state_t get_state();
int set_state(enum state_t);

int initialize_button_handlers();
void* pause_pressed_handler(void* ptr);
void* pause_unpressed_handler(void* ptr);
void* mode_pressed_handler(void* ptr);
void* mode_unpressed_handler(void* ptr);
int set_imu_interrupt_func(int (*func)(void));
void* imu_interrupt_handler(void* ptr);
