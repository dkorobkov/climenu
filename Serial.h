/*
 * Serial.h
 *
 * Created: 13.09.2013 13:16:03
 *  Author: User
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

#include <avr/pgmspace.h>


#define RX_BUF_LEN 255
#define TX_BUF_LEN 255

#define CALC_UBRRxL(a)  ( ( (F_CPU/(a*16) ) - 1) )
/*	// Расчёт из даташита при выключенном удвоителе частоты
	// Для 8 МГц максимальная почти безошибочная частота - 38400!
	BaudRate <<= 4; // *16
	BaudRate = F_CPU / BaudRate;
	BaudRate--;
*/

unsigned char UART0_HasData();

void UART0_init(unsigned long ubrr0l);

void UART0_SendByte( unsigned char data );

void UART0_SendArray(unsigned char* pData, unsigned char n);

void UART0_SendString_P(PGM_P psz);
void UART0_SendString(const char* psz);

unsigned char UART0_ReadByte();


#define putch UART0_SendByte
//#define printf UART0_SendString_P
#define getch  UART0_ReadByte
#define kbhit  UART0_HasData


#endif /* SERIAL_H_ */