#include <avr/io.h>
#include "uart.h"

/* initialize UART */
void uart_init (unsigned int baud)
{
	UBRR0H = (unsigned char) (baud>>8);
	UBRR0L = (unsigned char) baud;				//set the baud rate 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);	//enable UART receiver and transmitter 
}

/* Read and write functions */
unsigned char uart_receive_byte (void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);		// wait for incomming data 
	return UDR0;							// return the data 
}

void uart_transmit_byte (unsigned char data)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);	// wait for empty transmit buffer 
	UDR0 = data; 						// start transmittion 
}

void uart_transmit_string (const uchar_t* str)
{
	const uchar_t* i;
	for (i = str; *i; i++)
		uart_transmit_byte(*i);
}
