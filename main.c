#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "global.h"

#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

// Procyon headers
#include "spi.h"
#include "mmc.h"
#include "uart2.h"
#include "rprintf.h"

// Local headers
#include "lm74.h"

#ifdef __SIMULATOR__

#include "../simavr/include/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega644");

#include <stdio.h>
static int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                         _FDEV_SETUP_WRITE);

#define ifsim(...) __VA_ARGS__

#else
#define ifsim(...)
#endif

const char bootMessage[]	PROGMEM = "Initialising...";

#define sectBuffSize 512
u08 sectBuff[sectBuffSize];

void alive(void) {
	cbi(PORTA,PA0);
	_delay_ms(500);
	sbi(PORTA,PA0);
	_delay_ms(500);
}

//static void (*nisse)(unsigned char c);

void nisse(const prog_char str[]) {
	register char c;
	uart0SendByte('\n');
	uart0SendByte('\r');
	while((c = pgm_read_byte(str++)))
		uart0SendByte(c);
}

int main (void) {
	char tmpStringBuff[16];
	u16 temperature;
	PORTA = 0xFF;
	DDRA = 0xFF;


	uartInit();
	uartSetBaudRate(0,1200);
	rprintfInit(uart0SendByte);

	_delay_ms(1000);

	rprintfProgStr(bootMessage);
	rprintfCRLF();
	
	// Make sure that our large buffer is in .bss
	// if( (void *)sectBuff < __heap_start ) {
	// 	//Success
	// 	cbi(PORTA, PA0);
	// 	_delay_ms(500);
	// 	sbi(PORTA, PA0);
	// }
	//rprintfInit(uart_transmit_byte);

	// spiInit();
	mmcInit();
	// uart_transmit_byte(mmcReset());
	while(0x0 != mmcReset()) {
		_delay_ms(100);
	}

	sectBuff[0] = 'a';
	sectBuff[1] = 'b';
	sectBuff[2] = 'c';
	sectBuff[3] = 'd';
	sectBuff[4] = 'e';

	while(0x0 != mmcWrite(0x40000, sectBuff)) {
		_delay_ms(100);
	}
	memset(sectBuff, 0, sizeof(u08)*sectBuffSize);
	while(0x0 != mmcRead(0x40000,sectBuff)) {
		_delay_ms(100);
	}

	rprintfStr(sectBuff);
	rprintfCRLF();

	// set_sleep_mode(SLEEP_MODE_IDLE);
	// 
	for(;;) {
		temperature = read_temperature();
		format_temperature(tmpStringBuff, temperature);
		rprintfStr(tmpStringBuff);
		rprintfCRLF();
		// uart_transmit_byte(temperature >> 8);
		// uart_transmit_byte(temperature & 0xFF);
		// uart_transmit_string(newLine);
		_delay_ms(10000);
	}
}
