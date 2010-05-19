#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>
#include "uart.h"

void spiInit (void)
{
	// Set MOSI and SCK output, MISO input
	DDRB |= (1 << PINB5) | (1 << PINB7);
	DDRB &= ~(1 << PINB6);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (1 << DORD);
}

int main (void) {
	DDRA = 0xFF;
	PORTA = 0xFF;

	uart_init(51);
	uart_transmit_string("Booting SPI.\r\n");
	spiInit();

	uart_transmit_string("Testing temperature: ");
	while (1) {
		int temperature;

		//_delay_ms(1000);

		temperature = read_temperature();
		uart_transmit_byte(temperature);
		//uart_transmit_byte(temperature >> 8);
		//uart_transmit_byte(temperature & 0xFF);
		uart_transmit_string(" ; ");
	}

	return 0;
}
