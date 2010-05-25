#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "global.h"

#include <stdlib.h>
#include <util/delay.h>

// Procyon headers
#include "spi.h"
#include "mmc.h"
#include "rprintf.h"

// Local headers
#include "uart.h"
#include "lm74.h"

const char bootMessage[]	PROGMEM = "Initialising...";
const char newLine[]		PROGMEM = "\r\n";

u08 sectBuff[512];

void alive(void) {
	cbi(PORTA,PA0);
	_delay_ms(500);
	sbi(PORTA,PA0);
}
int main (void) {
	char tmpStringBuff[16];
	u08 i;
	u16 temperature;
	PORTA = 0xFF;
	DDRA = 0xFF;
	// _delay
	uart_init(51);
	uart_transmit_const_string(bootMessage);
	uart_transmit_const_string(newLine);
	
	// Make sure that our large buffer is in .bss
	if( sectBuff < __heap_start ) {
		//Success
		cbi(PORTA, PA0);
		_delay_ms(500);
		sbi(PORTA, PA0);
	}
	//rprintfInit(uart_transmit_byte);

	// spiInit();
	mmcInit();
	// uart_transmit_byte(mmcReset());
	if(0x0 != mmcReset()) {
		cbi(PORTA,PA0);
		// 	uart_transmit_const_string(bootMessage);
		// 	uart_transmit_const_string(newLine);
	}
	else {
		mmcRead(0,sectBuff);
		for(i = 0; i < 32; i++) {
			uart_transmit_byte(sectBuff[i]);
		}
	}

	// set_sleep_mode(SLEEP_MODE_IDLE);
	// 
	for(;;) {
		temperature = read_temperature();
		format_temperature(tmpStringBuff, temperature);
		uart_transmit_string(tmpStringBuff);
		uart_transmit_byte('\r');
		uart_transmit_byte('\n');
		// uart_transmit_byte(temperature >> 8);
		// uart_transmit_byte(temperature & 0xFF);
		// uart_transmit_string(newLine);
		_delay_ms(1000);
	}
}
