#ifndef _LM74_H
#define _LM74_H

#include <avr/io.h>

#include "global.h"


int format_temperature (char* buf, short temperature);
int read_temperature (void);
#endif
