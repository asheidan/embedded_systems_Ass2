#include <avr/io.h>
#include <avr/pgmspace.h>
#include "uart.h"

/* initialize UART */
void uart_init (unsigned int baud)
{
	UBRR0H = (unsigned char) (baud>>8);
	UBRR0L = (unsigned char) baud;				//set the baud rate 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);	//enable UART receiver and transmitter 
	
	/* Set frame format: 8data, 2stop bit */
	// UCSRC = (1<<USBS)|(3<<UCSZ0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	
	/* Enagle RC_Int */
	// UCSR0B |= (1<<RXCIE0);
}

/* Read and write functions */
u08 uart_receive_byte (void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);		// wait for incomming data 
	return UDR0;							// return the data 
}

void uart_transmit_byte (u08 data)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);	// wait for empty transmit buffer 
	UDR0 = data; 						// start transmittion 
}

void uart_transmit_string (const char* str)
{
	uint8_t i;
	for (i = 0; pgm_read_byte(&str[i]) != '\0'; i++) {
		uart_transmit_byte(pgm_read_byte(&str[i]));
	}
}
