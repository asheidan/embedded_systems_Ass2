#include <stdio.h>
#include <string.h>

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
	i += sprintf(&buf[i], "%d.", temperature >> TEMPERATURE_DECIMALS);
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

int main (void)
{
	const short tests[] = {
		0x4B07,
		0x3E87,
		0x0C87,
		0x000F,
		0x0007,
		0xFFFF,
		0xF387,
		0xE487,
		((23 << TEMPERATURE_DECIMALS) | 0x0004) << TEMPERATURE_START | 0x0007,
		(-((23 << TEMPERATURE_DECIMALS) | 0x0004)) << TEMPERATURE_START | 0x0007
	};
	const char* expected[] = {
		"150.0",
		"125.0",
		"25.0",
		"0.0625",
		"0.0",
		"-0.0625",
		"-25.0",
		"-55.0",
		"23.25",
		"-23.25"
	};
	char buf[20];
	int i;

	for (i = 0; i < sizeof(tests)/sizeof(*tests); i++) {
		printf("Testing 0x%hX.\n", tests[i]);
		memset(buf, sizeof(buf), 0);
		format_temperature(buf, tests[i]);
		if (strcmp(buf, expected[i]) != 0) {
			fprintf(stderr, "Failure: got \"%s\" expected \"%s\".\n", buf, expected[i]);
		}
	}

	return 0;
}

