// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"
#include "Piano.h"

unsigned char Index;
//const unsigned char SineWave[32] = {8,9,10,12,13,14,14,15,15,15,14,14,13,12,10,9,8,6,5,3,2,1,1,0,0,0,1,1,2,3,5,6,};
//const unsigned char SineWave[32] = {8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};
const unsigned char SineWave[32] = {5,5,4,1,1,3,8,11,11,9,4,2,5,11,15,13,9,7,5,5,6,8,8,7,4,3,3,3,3,4,5,5};
//const unsigned char SineWave[16] = {4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3};
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();          // Port B is DAC
  Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CTRL_R = 0x0007;// reload value
// this routine sets the RELOAD and starts SysTick
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
	NVIC_ST_CTRL_R = 0;
	GPIO_PORTB_DATA_R &= ~0xF;
	NVIC_ST_CTRL_R = 0x0007;
 // this routine stops the sound output
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){volatile unsigned long current_switch;
	current_switch = GPIO_PORTE_DATA_R&0x0F;
	if(current_switch == 0x00){
		GPIO_PORTB_DATA_R &= ~0x0F;
	}
	else{
	Index = (Index+1)&0x1F;
	DAC_Out(SineWave[Index]);
}
}
