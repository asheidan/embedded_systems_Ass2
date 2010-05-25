#include "lm74.h"

#include "spi.h"

int read_temperature(void) {
	u16 result;
	cbi(PORTA,LM74_CS);
	result = spiTransferWord(0);
	sbi(PORTA,LM74_CS);
	return (int)result;
}