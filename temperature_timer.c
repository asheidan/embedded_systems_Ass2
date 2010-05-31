#include "temperature_timer.h"
#include <string.h>
#include <avr/Interrupt.h>

#include "mmc.h"
#include "timer.h"
#include "rprintf.h"
#include "lm74.h"

#include "global.h"
#include "mmcfunctions.h"
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask


void init_temperature_timer (void)
{
	mmcInitCard();

	outb(TCCR1B, 1<<WGM12);
	outb(TCCR1B, (inb(TCCR1B) & ~TIMER_PRESCALE_MASK) | 4); // Prescaler 256
	OCR1A = 31250; // 1 int/sek
	sbi( TIMSK1, OCIE1A ); // Enable output compare
	sei();
}

void temperature_timer_interrupt (void)
{

	char buf[16];
	u08 i;
	short temperature;

	if(conf->current_sector > mmcSectorCount) {
		cbi(PORTA,PA0);
		return;
	}
	else {
		sbi(PORTA,PA0);
	}

  
	do {
		temperature = read_temperature();
	} while(temperature>>TEMPERATURE_POINT < -200);

	// Format temperature.
	format_temperature(buf, temperature);
	for (i = 0; buf[i] != '.'; i++);
	if (buf[i + 2] >= '5')
		buf[i + 1]++;
	buf[i + 2] = '\0';

	//cbi(PORTA, PA0);
	if (mmc_buf_i != 0 || conf->current_sector != START_SECTOR)
		write_char('\n');

	for (i = 0; buf[i] != '\0'; i++)
		write_char(buf[i]);

	write_char('\0');
	mmc_buf_i--;

	// Write to sector.
	mmcWrite(conf->current_sector, mmc_buf);

	//sbi(PORTA, PA0);
}

void write_char (char c)
{
	// At end of sector.
	if (mmc_buf_i == sizeof(mmc_buf)/sizeof(*mmc_buf)) {
		// Write the sector..
		mmcWrite(conf->current_sector, mmc_buf);
		conf->current_sector++;
		conf->check_sum = checksum(conf->current_sector);

		// Write new end sector to config.
		mmcWrite(CONFIG_SECTOR, config_buf);

		memset(mmc_buf, 0, sizeof(mmc_buf));
		mmc_buf_i = 0;
	}

	// Put char in sector buffer.
	mmc_buf[mmc_buf_i] = c;
	mmc_buf_i++;
}

ISR(TIMER1_COMPA_vect) {
	temperature_timer_interrupt();
}
