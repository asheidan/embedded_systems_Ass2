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
#include "uart2.h"
#include "rprintf.h"

// Local headers
#include "uart.h"
#include "lm74.h"

const char bootMessage[]	PROGMEM = "Initialising...";

extern void *__heap_start;
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
	uartInit();
	uartSetBaudRate(0,9600);
	rprintfInit(uart0SendByte);

	rprintfProgStr(bootMessage);
	rprintfCRLF();
	
	// Make sure that our large buffer is in .bss
	if( (void *)sectBuff < __heap_start ) {
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
			rprintfChar(sectBuff[i]);
		}
	}

	// set_sleep_mode(SLEEP_MODE_IDLE);
	// 
	for(;;) {
		temperature = read_temperature();
		format_temperature(tmpStringBuff, temperature);
		rprintfProgStr(tmpStringBuff);
		rprintfCRLF();
		// uart_transmit_byte(temperature >> 8);
		// uart_transmit_byte(temperature & 0xFF);
		// uart_transmit_string(newLine);
		_delay_ms(1000);
	}
}
