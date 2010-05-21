#include "lm74.h"

int read_temperature(void) {
	u16 result;
	cbi(PORTA,LM74_CS);
	result = spiTransferByte(0);
	return (int)result;
}