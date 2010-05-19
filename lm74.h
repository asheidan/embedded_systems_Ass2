#ifndef _LM74_H
#define _LM74_H

#include <avr/io.h>

#include "types.h"

#define LM74_CS PINA2

#ifndef SETBIT
#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#endif

#ifndef CLEARBIT
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#endif

int read_temperature (void);

#endif
