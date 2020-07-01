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

// Task 2 Macro Definitions
#define MASK 0x01

// Task 3 Macro Definitions
#define LEDPIN 6
#define SWITCHPIN 10

// Thread to read the joystick 
void read_joystick(void *arg){
	while(true){
		SystemInit(); 
		
		//pointing to block of registers for GPIO1 and output pins set in FIODIR (data direction resiter) to indicate that the pins are inputs

		LPC_GPIO1->FIODIR =((0<<JOYSTICKNORTH)); 
		LPC_GPIO1->FIODIR =((0<<JOYSTICKEAST));
		LPC_GPIO1->FIODIR =((0<<JOYSTICKSOUTH)); 
		LPC_GPIO1->FIODIR =((0<<JOYSTICKWEST)); 
		
		// infinite polling (keeps checking to see if data has been received)
		while(true){
			//FIOPIN used to read the pin 
			//logical condition: check if the joystick is pressed or not 
			if(!(LPC_GPIO1->FIOPIN>>JOYSTICKPRESS & MASK)){
				//checking for each direction of the joystick and printing out the position
				if(!(LPC_GPIO1->FIOPIN>>JOYSTICKNORTH & MASK)){
					printf("%s\n", "Joystick Position: North and Pressed"); 
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKEAST & MASK)){
					printf("%s\n", "Joystick Position: East and Pressed");
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKSOUTH & MASK)){
					printf("%s\n", "Joystick Position: South and Pressed");
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKWEST & MASK)){
					printf("%s\n", "Joystick Position: West and Pressed");
				}
				else{
					printf("%s\n", "Joystick Position: Center and Pressed");
				}
			}
			else{
				if(!(LPC_GPIO1->FIOPIN>>JOYSTICKNORTH & MASK)){
					printf("%s\n", "Joystick Position: North and Not Pressed"); 
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKEAST & MASK)){
					printf("%s\n", "Joystick Position: East and Not Pressed");
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKSOUTH & MASK)){
					printf("%s\n", "Joystick Position: South and Not Pressed");
				}
				else if(!(LPC_GPIO1->FIOPIN>>JOYSTICKWEST & MASK)){
					printf("%s\n", "Joystick Position: West and Not Pressed");
				}
				else{
					printf("%s\n", "Joystick Position: Center and Not Pressed");
				}
			}
			osThreadYield();
		}
	}	
}

// Thread to read values from the ADC and print them out to serial port 
void adc_2_serial(void *arg){

	while(true){
		SystemInit(); 
	//initialization (setting bit 12/adc bit)
		
		LPC_SC->PCONP |= (MASK << 12); 
		
		//set the pin select register (as analog rather than GPIO)
		//clear bit 
		LPC_PINCON->PINSEL1 &= ~(0x03 << 18); 
		LPC_PINCON->PINSEL1 |= (MASK << 18); 
		
		//set ADCR for correct input (potentiometer pin 2)
		// using bits 8-15 as an 8 bit binrary # to represent the divisor 
		//enable the adcr circuitry (enable bit = 21)
		LPC_ADC->ADCR = (1 << 2)|(4 << 8)|(1 << 21);
		
	// reading the adc 
		
		while (true) {
			// begin conversion
			LPC_ADC->ADCR |= (1 << 24);
			
			while ((LPC_ADC->ADGDR & 0x1000000000) != 0);
			int adc_val = (LPC_ADC->ADGDR & (0xFFF << 4)) >> 4; // 4 bit shift to the right, & with bit mask 
			printf("Converted Value: %d\n", adc_val);
			osThreadYield();
		}
	}
}

// Thread to toggle the LED
void toggle_led(void *arg){
	while(true){
		uint32_t currStatus; 
		SystemInit(); 
		
		LPC_GPIO2->FIODIR =(0<<SWITCHPIN);
		LPC_GPIO2->FIODIR =(1<<LEDPIN);
		
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
	osKernelInitialize(); 
	osThreadNew(toggle_led, NULL, NULL); 
	osThreadNew(adc_2_serial, NULL, NULL); 
	osThreadNew(read_joystick, NULL, NULL); 
	osKernelStart();  
}
