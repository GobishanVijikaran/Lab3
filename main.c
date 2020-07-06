#include <stdio.h>
#include <cmsis_os2.h>
#include <lpc17xx.h>
#include <stdbool.h>

// Task 1 Macro Definitions 
#define JOYSTICKNORTH 23
#define JOYSTICKEAST 24
#define JOYSTICKSOUTH 25 
#define JOYSTICKWEST 26
#define JOYSTICKPRESS 20
#define LEDPIN0 28
#define LEDPIN1 29
#define LEDPIN2 31
#define LEDPIN3 2
#define LEDPIN4 3

// Task 2 Macro Definitions
#define MASK 0x01

// Task 3 Macro Definitions
#define LEDPIN 6
#define SWITCHPIN 10

// Thread to read the joystick 
void read_joystick(void *arg){
		// infinite polling (keeps checking to see if data has been received)
		while(true){
			LPC_GPIO1->FIOCLR |= (1 << LEDPIN0);
		  LPC_GPIO1->FIOCLR |= (1 << LEDPIN1);
		  LPC_GPIO1->FIOCLR |= (1u << LEDPIN2);
		  LPC_GPIO2->FIOCLR |= (1 << LEDPIN3);
			
			//FIOPIN used to read the pin 
			//logical condition: check if the joystick is pressed or not 
			if(!(LPC_GPIO1->FIOPIN>>JOYSTICKPRESS & MASK)){
				LPC_GPIO2->FIOSET |=(1 << LEDPIN4);
			}
			else {
				LPC_GPIO2->FIOCLR |=(1 << LEDPIN4);
			}
			if(!(LPC_GPIO1->FIOPIN>>JOYSTICKNORTH & MASK)){
				LPC_GPIO1->FIOSET |= (1 << LEDPIN0);
			}
			else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKEAST & MASK)){
				LPC_GPIO1->FIOSET |= (1 << LEDPIN1);
			}
			else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKSOUTH & MASK)){
				LPC_GPIO1->FIOSET |= (1 << LEDPIN0);
				LPC_GPIO1->FIOSET |= (1 << LEDPIN1);
			}
			else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKWEST & MASK)){
				LPC_GPIO1->FIOSET |= (1u << LEDPIN2);
			}
			osDelay(25);
		}
	}	

// Thread to read values from the ADC and print them out to serial port 
void adc_2_serial(void *arg){

	while(true){ 
		//initialization (setting bit 12/adc bit)
		LPC_SC->PCONP |= (MASK << 12); 
		
		//set the pin select register (as analog rather than GPIO)
		LPC_PINCON->PINSEL1 &= ~(0x03 << 18); 
		LPC_PINCON->PINSEL1 |= (MASK << 18); 
		
		//set ADCR
		LPC_ADC->ADCR = (1 << 2)|(4 << 8)|(1 << 21);
		
	// reading the adc 
		
		while (true) {
			// begin conversion
			LPC_ADC->ADCR |= (1 << 24);
			
			while ((LPC_ADC->ADGDR & 0x1000000000) != 0);
			int adc_val = (LPC_ADC->ADGDR & (0xFFF << 4)) >> 4; // 4 bit shift to the right, & with bit mask 
			printf("%s %d\n",(char *) arg, adc_val);
			osThreadYield();
		}
	}
}

// Thread to toggle the LED
void toggle_led(void *arg){
	while(true){
		uint32_t currStatus; 		
		while(true){ 
			currStatus = (LPC_GPIO2->FIOPIN & (1<<SWITCHPIN)); 
			if(!currStatus){ 
				LPC_GPIO2->FIOSET |=(1<<LEDPIN); 
			}
			else{
				LPC_GPIO2->FIOCLR =(1<<LEDPIN); 
			}
			osThreadYield();
		}
	}	
}

int main(){
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
	osKernelInitialize(); 
  osThreadNew(toggle_led, NULL, NULL); 
  osThreadNew(adc_2_serial, "Converted Value:", NULL); 
	osThreadNew(read_joystick, NULL, NULL); 
	osKernelStart();  
}
