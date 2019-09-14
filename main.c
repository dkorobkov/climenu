/*
 * SerialMenu644.c
 *
 * Created: 13.09.2013 12:52:05
 *  Author: User
 */ 

#include "f_cpu.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Menu.h"
#include "Serial.h"


int main(void)
{
	UART0_init( CALC_UBRRxL(38400) );

	sei();
	
	while(1)
	{
	
		int timeout = 1; // 0 - не увеличиваем, если хоть раз зашли в меню

		UART0_SendString_P( PSTR("Press ENTER to enter menu\r\n") );

		#define MAXTIMEOUT 100

		while(timeout < MAXTIMEOUT )
		{

			switch(ProcessMenu())
			{
				case MR_WORKING:
				timeout = 0;
				break;
				case MR_NOTHINGPRESSED:
				if(timeout > 0)
				timeout++;
				if((timeout % 10) == 9)
				UART0_SendString_P(PSTR("."));
				break;
				case MR_EXITMENU:
				timeout = MAXTIMEOUT; // exit from loop
				break;
				default:
				UART0_SendString_P(PSTR("ERROR: switch(ProcessMenu()), default"));
				break;
			}
			_delay_ms(100);
		}

		UART0_SendString_P(PSTR("main() finished; press ESC\n"));
		while(getch() != 27);
		UART0_SendString_P(PSTR("DONE, RELOADING\n"));
	}
	return 0;
}

