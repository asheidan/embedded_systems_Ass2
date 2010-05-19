#include <avr/io.h>
#ifndef F_CPU
#define F_CPU 1000000
#endif
#include <util/delay.h>

#include "uart.h"

int main(void) {
	DDRA = 0xFF;
	for(;;) {
		PORTA = 0xFF;
		_delay_ms(500);
		TransmitByte('A');
		PORTA = 0x00;
		_delay_ms(500);
		TransmitByte('B');
	}
}