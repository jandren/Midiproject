/*
 * REC.h
 *
 * Created: 2015-11-23 14:57:27
 *  Author: Jakob Andren & SJ
 */ 
#include <avr/io.h>

#ifndef REC_H_
#define REC_H_

// Global variables
extern volatile uint8_t volume;

// Functions
bool REC_process(uint8_t switches, uint8_t command, uint8_t tone);
void REC_ISR(uint16_t time);
void REC_add(uint8_t command, uint8_t tone);

void REC_start(void);
void REC_stop(void);
void REC_play(uint8_t switches);


#endif /* REC_H_ */