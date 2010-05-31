#ifndef _TEMPERATURE_TIMER_H
#define _TEMPERATURE_TIMER_H

#include "avrlib/timer.h"

#define CONFIG_SECTOR 0
#define START_SECTOR (CONFIG_SECTOR + 1)

#define VERSION_NUMBER 0x01
typedef struct config {
	u08 version_number;
	u32 current_sector;
	u08 check_sum;
} config;

void init_temperature_timer (void);
void temperature_timer_interrupt (void);
void write_char (char c);

#endif
