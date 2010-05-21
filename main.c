#include <avr/io.h>

#include "global.h"

#include <util/delay.h>
#include "uart.h"


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
