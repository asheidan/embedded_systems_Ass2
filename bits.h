#ifndef _BITS_H_
#define _BITS_H_

#ifndef SETBIT
#define	SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#endif

#ifndef CLEARBIT
#define	CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#endif

#endif