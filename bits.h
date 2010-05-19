#ifndef _BITS_H_
#define _BITS_H_

#define	SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define	CLEARBIT(ADDRESS,BIT)	(ADDRESS &= ~(1<<BIT))

#endif