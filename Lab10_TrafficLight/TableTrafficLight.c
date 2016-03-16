// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))

#define LIGHT                   (*((volatile unsigned long *)0x400050FC))
#define PED_LIGHT               (*((volatile unsigned long *)0x40025028))
//#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // bits 5-0
//#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
//#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
//#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
//#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
//#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
//#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002400C)) // bits 2-0
#define SENSOR                  (*((volatile unsigned long *)0x4002401C))

//#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
//#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
//#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
//#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
//#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
//#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
//#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
//#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

void SysTick_Init(void);
void SysTick_Wait(unsigned long);
void SysTick_Wait10ms(unsigned long);

// Linked data structure
struct State {
  unsigned long Out; 
	unsigned long Out2;
  unsigned long Time;  
  unsigned long Next[9];}; 
typedef const struct State STyp;
#define goN   0
#define waitN 1
#define goE   2
#define waitE 3
#define walk  4
#define flash 5
#define off   6
#define flash2 7
#define off2  8
	
STyp FSM[9]={
 {0x21,0x02,10,{goN,waitN,goN,goN,waitN,goN,goN,waitN}}, //goN 
 {0x22,0x02,10,{goE,goE,waitN, waitN, walk, waitN, waitN,goE}}, //waitN
 {0x0C,0x02,10,{goE,goE,waitE,goE,waitE,goE,goE,waitE}}, //goE
 {0x14,0x02,10,{goN,waitE,goN,waitE,walk,waitE, waitE, walk}}, //waitE
 {0x24,0x08,10,{walk, flash,flash,walk,walk,walk,walk,flash}}, //walk
 {0x24,0x02,10,{flash, off, off ,walk,walk,walk,walk,off}}, //flash
 {0x24,0x00,10,{off,flash2,flash2,off,off,off,off,flash2}}, //off
 {0x24,0x02,10,{flash2,off2,off2,flash2,flash2,flash2,flash2,off2}}, //flash2
 {0x24,0x02,10,{off2,goE,goN,off2,off2,off2,off2,goN}} //off2
};
unsigned long S;  // index to the current state 
unsigned long Input; 

int main(void) {volatile unsigned long delay;
SysTick_Init();
SYSCTL_RCGC2_R |= 0x32;      // 1) B E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE1-0
	
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
	GPIO_PORTF_AMSEL_R &= ~0x0A;
	GPIO_PORTF_PCTL_R &= ~0x000FFF0;
	GPIO_PORTF_DIR_R |= 0x0A;
	GPIO_PORTF_AFSEL_R &= ~0x0A;
	GPIO_PORTF_DEN_R |= 0x0A;
	
  S = goN;  
  while(1){
    LIGHT = FSM[S].Out;  // set lights
		PED_LIGHT = FSM[S].Out2;
    SysTick_Wait10ms(FSM[S].Time);
    Input = SENSOR;     // read sensors
    S = FSM[S].Next[Input];  
  }
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 10000us equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}


