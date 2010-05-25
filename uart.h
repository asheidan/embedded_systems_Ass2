#ifndef _UART_H
#define _UART_H

#include "global.h"

void uart_init (unsigned int baud);
u08 uart_receive_byte (void);
void uart_transmit_byte (u08 data);
void uart_transmit_const_string (const char* str);
void uart_transmit_string (const char* str);

#endif
