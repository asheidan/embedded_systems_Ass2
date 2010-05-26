#include <stdlib.h>
#include "lm74.h"
#include "lm74conf.h"

#include "spi.h"

#define TEMPERATURE_POINT 7
#define TEMPERATURE_START 3
#define TEMPERATURE_DECIMALS (TEMPERATURE_POINT - TEMPERATURE_START)

int format_temperature (char* buf, short temperature)
{
	int i = 0;

	temperature >>= TEMPERATURE_START;

	// Handle negative values.
	if (temperature & 0x8000) {
		buf[i] = '-';
		i++;
		temperature = -temperature;
	}

	// Add integral part.
	itoa(temperature>>TEMPERATURE_DECIMALS, &buf[i], 10);
	for(;buf[i] != 0; i++);
	buf[i] = '.';
	i++;
	// i += sprintf(&buf[i], "%d.", temperature >> TEMPERATURE_DECIMALS);
	temperature &= ~(~0 << TEMPERATURE_DECIMALS);

	if (!temperature) {
		buf[i] = '0';
		i++;
	} else {
		// Add fractional part.
		while (temperature) {
			buf[i] = '0' + ((temperature*10) >> TEMPERATURE_DECIMALS);
			i++;
			temperature = (temperature*10) & ~(~0 << TEMPERATURE_DECIMALS);
		}
	}

	buf[i] = '\0';
	i++;

	return i;
}


int read_temperature(void) {
	u16 result;
	cbi(LM74_CS_PORT,LM74_CS_PIN);
	result = spiTransferWord(0);
	sbi(LM74_CS_PORT,LM74_CS_PIN);
	return (int)result;
}
