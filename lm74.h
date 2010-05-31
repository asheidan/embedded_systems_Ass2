#ifndef _LM74_H
#define _LM74_H

#include <avr/io.h>

#include "global.h"

#define TEMPERATURE_POINT 7

void lm74Init(void);
u08 format_temperature (char* buf, short temperature);
short read_temperature (void);
#endif
