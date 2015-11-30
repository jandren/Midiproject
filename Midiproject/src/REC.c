/*
 * REC.c
 *
 * Saves data in arrays and its time, every note will consist of two commands, press and release. 
 *
 *
 * Created: 2015-11-23 14:57:16
 *  Author: Jakob Andren & SJ
 */ 

#include "REC.h"

volatile uint8_t volume = 100;
volatile uint16_t software_time = 0;
volatile uint16_t software_comp = 0;


uint8_t com[256];
uint8_t tones[256];
uint8_t vol[256];
uint16_t rec_time[256];
uint8_t rec_index = 0;
uint8_t REC = 0;
uint8_t PLAY = 0;

void init_Timer1(void)				// for recording
{
	TCCR1A = 0b00000000;			// normal mode
	TCCR1B = 0b00000011;			// prescaler 64
	OCR1A = 125;					// output compare, 65525*1024/(8*10^6) = 8.4s at max
}

ISR(TIMER1_COMPA_vect)
{
	software_time++;
	if(software_time == software_comp)
	{
		REC_ISR(software_time);
	}
}

void TIME_Set_ISR(uint16_t time)
{
	software_comp = time;
}

void TIME_reset(void)
{
	software_time = 0;
}


void REC_process(uint8_t switches, uint8_t command, uint8_t tone){
	
	if(REC == 1){ // If recording already
		if(switches & 0x80){
			REC_add(command, tone);
		}
		else REC_stop();
	}
	
	else{
		if(switches & 0x80){ // Start recording and save first command
			 REC_start();
			 REC_add(command, tone);
		}
	}

	
	
}

void REC_ISR(uint16_t time){
	if(PLAY){
		//MIDI_send(com[rec_index], tones[rec_index], vol[rec_index]);
		rec_index++;
		
		// Last record will be of time zero
		// Restart the playback in that case
		if(rec_time[rec_index] == 0){
			rec_index = 0;
			software_time = 0;//TIME_reset();
		}

		software_comp = rec_time[rec_index]; //TIME_Set_ISR(rec_time[rec_index]);
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
	rec_time[rec_index] = 0;
	REC = 0;
}

void REC_play(uint8_t switches)
// For starting the play. Always from the start
{
	if(PLAY){
		if(switches & 0x40){  // If play channel one is on
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