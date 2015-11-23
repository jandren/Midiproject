/*
 * avrMIDI.h
 *
 * Includes UART and MIDI instructions
 *
 * Created: 2015-11-23 11:31:45
 *  Author: Jakob Andren, SJ
 */ 


#ifndef AVRMIDI_H_
#define AVRMIDI_H_



void send_MIDI(uint8_t command, uint8_t tone, uint8_t volume);



#endif /* AVRMIDI_H_ */