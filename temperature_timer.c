#include "temperature_timer.h"
#include <string.h>

#include "mmc.h"
#include "lm74.h"

u32 current_sector;
char mmc_buf[512];
int mmc_buf_i;
int overflows;

void init_temperature_timer (void)
{
	mmcRead(CONFIG_SECTOR, mmc_buf);
	if (mmc_buf[0] + mmc_buf[1] + mmc_buf[2] + mmc_buf[3] == mmc_buf[4]) {
		// Config validates, use end sector.
		current_sector = *(u32*) mmc_buf;
		mmcRead(current_sector, mmc_buf);
		mmc_buf_i = strlen(mmc_buf);
	} else {
		// Start anew.
		current_sector = START_SECTOR;
		mmc_buf_i = 0;
	}

	overflows = 0;
	timer0Init();
	timer0SetPrescaler(8); // Clock-div 512
	timerAttach(0, temperature_timer_interrupt);
}

void temperature_timer_interrupt (void)
{
	overflows++;
	if (overflows != 61) // 256/(8*10^6/512) = 61 overflows per second.
		return;

	overflows = 0;
	char buf[16];
	int i;
	int temperature = read_temperature();

	format_temperature(buf, temperature);

	cbi(PORTA, PA0);
	if (mmc_buf_i != 0 || current_sector != START_SECTOR)
		write_char('\n');

	for (i = 0; buf[i] != '\0'; i++)
		write_char(buf[i]);

	write_char('\0');
	mmc_buf_i--;

	// Write to sector.
	mmcWrite(current_sector, mmc_buf);

	sbi(PORTA, PA0);
}

void write_char (char c)
{
	// At end of sector.
	if (mmc_buf_i == sizeof(mmc_buf)/sizeof(*mmc_buf)) {
		// Write the sector..
		mmcWrite(current_sector, mmc_buf);
		current_sector++;

		// Write new end sector to config.
		mmcRead(CONFIG_SECTOR, mmc_buf);
		*(u32*) mmc_buf = current_sector;
		mmc_buf[4] = mmc_buf[0] + mmc_buf[1] + mmc_buf[2] + mmc_buf[3];
		mmcWrite(CONFIG_SECTOR, mmc_buf);

		memset(mmc_buf, 0, sizeof(mmc_buf));
		mmc_buf_i = 0;
	}

	// Put char in sector buffer.
	mmc_buf[mmc_buf_i] = c;
	mmc_buf_i++;
}
