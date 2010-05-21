#ifndef _LM74_H
#define _LM74_H

#include <avr/io.h>

#include "global.h"

#define LM74_CS PINA2

int read_temperature (void);

#endif
