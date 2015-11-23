/*
 * REC.c
 *
 * Created: 2015-11-23 14:57:16
 *  Author: Jakob Andren & SJ
 */ 

#include "REC.h"
#include <avr/io.h>

volatile uint8_t volume = 100;

uint8_t com[256];
uint8_t tones[256];
uint8_t vol[256];
uint16_t rec_time[256];
uint8_t rec_index = 0;
uint8_t REC = 0;
uint8_t PLAY = 0;


void REC_ISR(uint16_t time){
	if(PLAY){
		//MIDI_send(com[rec_index], tones[rec_index], vol[rec_index]);
		rec_index++;
		
		// Last record will be of time zero
		// Restart the playback in that case
		if(rec_time[rec_index] == 0){
			rec_index = 0;
			//time_reset();
		}

		//time_set_isr(rec_time[rec_index]);
	}
	
	// There will be no new interrupt if PLAY == 0
}

void REC_add(uint8_t command, uint8_t tone){
	
	com[rec_index]  = command;
	tones[rec_index] = tone;
	vol[rec_index] = volume;
	rec_time[rec_index] = 0; //time_read();
	
	rec_index++;	
}

void REC_start(void){
	REC = 1;
	rec_index = 0;
	//time_reset();
}

void REC_stop(void){
	com[rec_index]  = 0;
	tones[rec_index] = 0;
	vol[rec_index] = 0;
	rec_time[rec_index] = 0; //time_read();
	REC = 0;
}

void REC_play(uint8_t switches)
// For starting the play. All ways from the start
{
	if(PLAY){
		if(switches & 0x40){
			PLAY = 1;
			rec_index = 0;
			//time_reset();
			//time_set_isr(0);
		}
		else{
			PLAY = 0;
		}
	}
	
}