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

volatile uint8_t volume = 200;
volatile uint16_t software_time = 0;
volatile uint16_t software_comp = 100;


uint8_t com[256];
uint8_t tones[256];
uint8_t vol[256];
uint16_t rec_time[256];
uint8_t rec_index = 0;
uint8_t REC = 0;
uint8_t PLAY = 0;

void init_Timer1(void)				// for recording
{
	TCCR1A = 0x00;		  // Normal timer operation
	TIMSK1 = (1<<OCIE0A); // Interrupt on compare register A
	TCNT1 =  0;			  // Set counter
	OCR1A = 125;		  // 100Hz interrupts to start with 
	
	// Start timer
	TCCR1B	= (1<<CS10); // No Pre-scaler for maximum resolution even at high freq
}

ISR(TIMER1_COMPA_vect)
{
	software_time++;
	PORTB = ~rec_index; //((PLAY<<6) | (REC<<7) | rec_index);
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

uint16_t TIME_read(void)
{
	return software_time;
}

void REC_ISR(uint16_t time){
	if(PLAY){
		MIDI_send(com[rec_index], tones[rec_index], vol[rec_index]);
		//PORTB = ~rec_index;
		
		// Last record will be of time zero
		// Restart the playback in that case
		if(com[rec_index] == 0){
			MIDI_send(com[0], tones[0], vol[0]);
			
			rec_index = 1;
			TIME_reset();
			
		}
		else{
			rec_index++;
		}
		
		TIME_Set_ISR(rec_time[rec_index]);	
	
	}
	
	// There will be no new interrupt if PLAY == 0
}

void REC_add(uint8_t command, uint8_t tone){
	
	if(REC){ // Recording 
		com[rec_index]  = command;
		tones[rec_index] = tone;
		vol[rec_index] = volume;
		
		// Recording starts on first tone, so set time = 0 and reset timer
		if(rec_index == 0){
			rec_time[rec_index] = 0;
			TIME_reset();
			
			
			
		}
		
		else { rec_time[rec_index] = TIME_read();}
			
		rec_index++;
	}
}

void REC_start(void){
	REC = 1;
	rec_index = 0;
	TIME_reset();
}

void REC_stop(void){
	// always record both on AND off commands = even number
	if(rec_index & 0x01){ // if not even
		com[rec_index]  = (com[rec_index-1] & 0b11101111); 
		tones[rec_index] = tones[rec_index-1];
		vol[rec_index] = volume;
		rec_index++;
	}
	
	com[rec_index]  = 0;
	tones[rec_index] = 0;
	vol[rec_index] = 0;
	rec_time[rec_index] = TIME_read();
	REC = 0;
}

void REC_state(uint8_t switches)
// For starting the play. Always from the start
{
	if(switches & 0x40){
		if(!PLAY){  // If play channel one is on
			PLAY = 1;
			
			// First note is on time = 0 which doesn't exist in our approach,
			// Therefor call the ICR for the first tone and then reset timer.
			rec_index = 0;
			REC_ISR(0);
			TIME_reset();
		}
	}
	else{ // If should be off
		if(PLAY){ // Turn off if on
			PLAY = 0;
		}
	}
	
	if(switches & 0x80){ // Recording?
		if(REC == 0){	 // Change from before?
			REC_start();
		}
	}
	else{
		if(REC == 1){ // If rec on, but swithces not in that position, stop
			REC_stop();
		}
	}
}