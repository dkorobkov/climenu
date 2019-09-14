/*
 * Serial.c
 *
 * Created: 13.09.2013 13:14:49
 *  Author: User
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "Serial.h"

volatile unsigned char RxBuf[RX_BUF_LEN];
volatile unsigned char idxRxBufWr = 0;
volatile unsigned char idxRxBufRd = 0;

volatile unsigned char TxBuf[TX_BUF_LEN];
volatile unsigned char idxTxBufWr = 0;
volatile unsigned char idxTxBufRd = 0;


#if RX_BUF_LEN >= 256
#error idxRxBuf must be USHORT then
#endif
#if TX_BUF_LEN >= 256
#error idxRxBuf must be USHORT then
#endif

/*#define UART0_rx_disable()				{cbi(UCSR0B, RXEN0);}	// disable receiver
#define UART0_rx_enable()				{sbi(UCSR0B, RXEN0);}	// enable receiver
#define UART0_rx_int_disable()			{cbi(UCSR0B, RXCIE0);}	// disable interrupt on receive    
#define UART0_rx_int_enable()			{sbi(UCSR0B, RXCIE0);}	// enable interrupt on receive	
#define UART0_txempty_int_disable()     {cbi(UCSR0B, UDRIE0);}	// disable interrupt on TX buf empty
#define UART0_txempty_int_enable()	    {sbi(UCSR0B, UDRIE0);}	// enable interrupt on TX buf empty
#define UART0_txcompl_int_disable()     {cbi(UCSR0B, TXCIE0);}	// disable interrupt on TX complete
#define UART0_txcompl_int_enable()	    {sbi(UCSR0B, TXCIE0);}	// enable interrupt on TX complete
*/
#define UART0_rx_disable()				{UCSR0B &= ~(_BV(RXEN0));}	// disable receiver
#define UART0_rx_enable()				{UCSR0B |= _BV(RXEN0));}	// enable receiver
#define UART0_rx_int_disable()			{UCSR0B &= ~(_BV(RXCIE0));}	// disable interrupt on receive
#define UART0_rx_int_enable()			{UCSR0B |= _BV(RXCIE0);}	// enable interrupt on receive
#define UART0_txempty_int_disable()     {UCSR0B &= ~(_BV(UDRIE0));}	// disable interrupt on TX buf empty
#define UART0_txempty_int_enable()	    {UCSR0B |= _BV(UDRIE0);}	// enable interrupt on TX buf empty
#define UART0_txcompl_int_disable()     {UCSR0B &= ~(_BV(TXCIE0));}	// disable interrupt on TX complete
#define UART0_txcompl_int_enable()	    {UCSR0B |= _BV(TXCIE0);}	// enable interrupt on TX complete


// Interrupt when byte arrives
ISR(USART0_RX_vect)
{
	RxBuf[idxRxBufWr++] = UDR0;
	if(idxRxBufWr >= RX_BUF_LEN)
		idxRxBufWr = 0;
}

// Interrupt when UDR0 is empty and ready to receive next byte for sendout
ISR(USART0_UDRE_vect)
{
	// ≈сли есть что послать
	if(idxTxBufRd != idxTxBufWr)
		UDR0 = TxBuf[idxTxBufRd++];

	if(idxTxBufRd >= TX_BUF_LEN)
		idxTxBufRd = 0;
	// ≈сли байты дл€ передачи кончились, нам больше не интересно, когда можно посылать
	if(idxTxBufRd == idxTxBufWr)
		UART0_txempty_int_disable();
		
}

void UART0_SendByte( unsigned char data )
{
	// ≈сли регистр дл€ передачи пустой, то мы можем положить этот байт пр€мо сейчас.
	// »наче говорим, что хотим получить прерывание, когда он освободитс€, и кладЄм байт
	// в очередь.
	if( UCSR0A & (1<<UDRE0))  // буфер пустой, пишем пр€мо сейчас
	{
		UDR0 = data;
	}
	else
	{
		TxBuf[idxTxBufWr] = data;
		idxTxBufWr++;
		if(idxTxBufWr >= TX_BUF_LEN)
			idxTxBufWr = 0;
		// разрешаем прерывание дл€ вывода очередного байта из буфера
		UART0_txempty_int_enable();	
	}
}

void UART0_SendArray(unsigned char* pData, unsigned char n)
{
	for(unsigned char i = 0; i < n; i++)
		UART0_SendByte(pData[i]);
}

void UART0_SendString_P(const char* PROGMEM psz)
{
	unsigned char len = strlen_P(psz);
	for(unsigned char i = 0; i < len; i++)
		UART0_SendByte(pgm_read_byte(&(psz[i])));
}

void UART0_SendString(const char* psz)
{
	unsigned char len = strlen(psz);
	for(unsigned char i = 0; i < len; i++)
		UART0_SendByte( psz[i] );
}


// Use CALC_UBRRxL(baud) to provide correct value
void UART0_init(unsigned long ubrr0l)
{
	UBRR0L = ubrr0l; //129 = 9600@20MHz;	      // 9600
	
	// RX enable, TX enable, RX complete interrupt enable
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);	      
	
	UCSR0C = (0<<UMSEL01) | (0<<UMSEL00) |		// Async UART
			(0<<UPM01) | (0<<UPM00) |			// No parity
			(0<<USBS0) |						// 1 stop bit
			(0<<UCSZ02) | (1<<UCSZ01) | (1<<UCSZ00); // 8 bits in byte
}

// ¬озвращает число байт в буфере
unsigned char UART0_HasData()
{
	int n = idxRxBufWr - idxRxBufRd;
	if(n < 0) n += RX_BUF_LEN;
	return (unsigned char)n;
}

// ¬озвращает следующий байт из буфера
unsigned char UART0_ReadByte()
{
	if(idxRxBufWr == idxRxBufRd)
		return 0;
		
	unsigned char uc = RxBuf[idxRxBufRd++];
	
	if(idxRxBufRd >= RX_BUF_LEN)
		idxRxBufRd = 0;
	
	return uc;
}


