#ifndef _UART_H
#define _UART_H

#include "types.h"

void uart_init (unsigned int baud);
uchar_t uart_receive_byte (void);
void uart_transmit_byte (uchar_t data);
void uart_transmit_string (const uchar_t* str);

#endif
