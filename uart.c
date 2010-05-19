
//CPU freq 1 MHz

#include <avr/io.h> 

#include "uart.h"
#include "bits.h"

//0x55FF
#define	UART_SYNC_DATA	"U\xFF"

/* initialize UART */
void InitUART( unsigned int baud ) {
	// DDRD = 0xFF; // Set Datadirection to output
	// InitUART(51);			// 1200 Baud, 1 Mhz
	
	// DDRD = 0xFF;			// output
 	
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;				//set the baud rate 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);	//enable UART receiver and transmitter 
	
	/* Set frame format: 8data, 2stop bit */
	// UCSRC = (1<<USBS)|(3<<UCSZ0);
	UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
	
	/* Enagle RC_Int */
	UCSR0B |= (1<<RXCIE0);
}

/* Read and write functions */
unsigned char ReceiveByte( void ) {
	loop_until_bit_is_set(UCSR0A,RXC0);		// wait for incomming data
	unsigned char t = UDR0;
	// TransmitByte(t);
		return t; //UDR;							// return the data 
}

void TransmitByte( unsigned char data ) {
	loop_until_bit_is_set(UCSR0A,UDRE0);	// wait for empty transmit buffer 
	UDR0 = data; 						// start transmittion 
}

void TransmitString( char* data ) {
	char *p;
	for(p = data; *p != 0; p++) {
		TransmitByte(*p);
	}
}

