#include <stdio.h>
#include <cmsis_os2.h>


// Each function is essentially a new thread (each thread has a void *arg parameter even if there arent any 

// Thread to read the joystick 

// Thread to read values from the ADC and print them out to serial port 

// Thread to toggle the LED


int main(){
	osKernelInitialize(); 
	osKernelStart();  
}