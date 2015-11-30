/*
 * REC.h
 *
 * Created: 2015-11-23 14:57:27
 *  Author: Jakob Andren & SJ
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef REC_H_
#define REC_H_

// Global variables
extern volatile uint16_t software_time;
extern volatile uint16_t software_comp;

extern volatile uint8_t volume;
extern volatile uint16_t software_time;
extern volatile uint16_t software_comp;


// Functions
void init_Timer1(void);
void TIME_Set_ISR(uint16_t time);
void TIME_reset(void);

void REC_process(uint8_t switches, uint8_t command, uint8_t tone);
void REC_ISR(uint16_t time);
void REC_add(uint8_t command, uint8_t tone);

void REC_start(void);
void REC_stop(void);
void REC_play(uint8_t switches);


#endif /* REC_H_ */