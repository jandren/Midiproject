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
volatile uint16_t software_comp = 100;


uint8_t com[5] = {'A', 'B', 'C', 'D', 'F'};
uint8_t tones[256];
uint8_t vol[256];
uint16_t rec_time[5] = {100, 200, 300, 400, 0};
uint8_t rec_index = 0;
uint8_t REC = 0;
uint8_t PLAY = 1;

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
	//UART_out(software_time);
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
		UART_out(com[rec_index]);
		PORTB = ~rec_index;
		rec_index++;
		
		
		// Last record will be of time zero
		// Restart the playback in that case
		if(rec_time[rec_index] == 0){
			rec_index = 0;
			TIME_Set_ISR(rec_time[0]);
			TIME_reset();
		}
		else{
			TIME_Set_ISR(rec_time[rec_index]);
		}
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
	if(switches & 0x40){
		if(!PLAY){  // If play channel one is on
			PLAY = 1;
			rec_index = 0;
			TIME_Set_ISR(rec_time[0]);
			TIME_reset();
		}
	}
	else{
		if(PLAY){
			PLAY = 0;
		}
	}
}