#include "lm74.h"

#define F_CPU 1000000
#include <util/delay.h>

uchar_t spiTransferByte (uchar_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF))) {
		_delay_ms(100);
		PORTA ^= 1 << PINA3;
	}
	return SPDR;
}


int read_temperature (void)
{
	int result;

	CLEARBIT(PORTA, LM74_CS);
	result = spiTransferByte(0);
	//result |= spiTransferByte(0) << 8;
	SETBIT(PORTA, LM74_CS);

	return result;
}
