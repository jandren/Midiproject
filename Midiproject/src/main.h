/*
 * main.h
 *
 * Created: 2015-11-23 11:42:43
 *  Author: ASUS
 */ 


#ifndef MAIN_H_
#define MAIN_H_

void UART_out(uint8_t ch);
void Init_UART0(uint16_t BAUDRATE);
void Init_ports(void);
uint8_t Conversion(uint8_t pressed, uint8_t switches);


// global variables
volatile uint16_t rx_ch = 0xFF;

#endif /* MAIN_H_ */