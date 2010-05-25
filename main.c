#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "global.h"

#include <util/delay.h>

// Procyon headers
#include "spi.h"

// Local headers
#include "uart.h"
#include "lm74.h"

const char bootMessage[]	PROGMEM = "Initialising...";
const char newLine[]		PROGMEM = "\r\n";

int main (void) {
	char buff[16];
	u16 temperature;
	PORTA = 0xFF;
	DDRA = 0xFF;
	// _delay
	uart_init(51);
	spiInit();
	uart_transmit_const_string(bootMessage);
	uart_transmit_const_string(newLine);
	// set_sleep_mode(SLEEP_MODE_IDLE);
	// 
	for(;;) {
		_delay_ms(1000);
		temperature = read_temperature();
		format_temperature(buff, temperature);
		uart_transmit_string(buff);
		uart_transmit_byte('\r');
		uart_transmit_byte('\n');
		// uart_transmit_byte(temperature >> 8);
		// uart_transmit_byte(temperature & 0xFF);
		// uart_transmit_string(newLine);
	}
}
