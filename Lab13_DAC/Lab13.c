// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
	DAC_Init();
	
  while(1){ volatile unsigned long current_switch;
	current_switch = Piano_In();
	if (current_switch == 0x01){
	EnableInterrupts();
	Sound_Tone(4780);
	}
	else if (current_switch == 0x02){
	EnableInterrupts();
	Sound_Tone(4259);
	}
	else if (current_switch == 0x04){
	EnableInterrupts();
	Sound_Tone(3794);
	}
	else if (current_switch == 0x08){
  EnableInterrupts();
	Sound_Tone(3189);
	}
//	else if (current_switch == 0x00){
//	Sound_Off();
//	DisableInterrupts();
//	}
	delay(10);
  }  
}	



// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


