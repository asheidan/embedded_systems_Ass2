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
#include "mmcconf.h"
#include "mmc.h"
#include "uart2.h"
#include "rprintf.h"
#include "net/netstack.h"

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
u32 mmcSectorCount;

void alive(void) {
	cbi(PORTA,PA0);
	_delay_ms(500);
	sbi(PORTA,PA0);
	_delay_ms(500);
}

u32 mmcExtractSectorsFromCSD(u08 *buff) {
	u16 c_size = 0;
	u08 c_mult = 0;
	c_size |= buff[0x06] & 0x03;
	c_size <<= 8;
	c_size |= buff[0x07];
	c_size <<= 2;
	c_size |= buff[0x08] >> 6;

	rprintfu16(c_size);
	rprintfCRLF();

	c_mult |= buff[0x09] & 0x03;
	c_mult <<= 1;
	c_mult |= buff[0x0A] >> 7;

	rprintfu08(c_mult);
	rprintfCRLF();

	// Sectorsize
	// rprintfu16((u16)1<<(buff[0x05] & 0x0F));
	// rprintfCRLF();

	return (u32)(c_size + 1) * ((u32)1 << (c_mult + 2));
}

int main (void) {
	char tmpStringBuff[16];
	u16 temperature;
	PORTA = 0xFF;
	DDRA = 0xFF;
	u08 i;
	//u08 *p;

	uartInit();
	uartSetBaudRate(0,9600);
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

	spiInit();
	lm74Init();
	mmcInit();
	// uart_transmit_byte(mmcReset());
	mmcReset();
	// while(0x0 != mmcReset()) {
	// 	#ifdef MMC_DEBUG
	// 	rprintf("MMC Reset Failed, retrying in 500ms\r\n");
	// 	#endif
	// 	_delay_ms(500);
	// }

	// Reading card size
	cbi(MMC_CS_PORT,MMC_CS_PIN);
	i = mmcCommand(MMC_SEND_CSD,0);

	if(0x00 == i) {
		while(spiTransferByte(0xFF) != MMC_STARTBLOCK_READ);
		for(i = 0; i < 0x10; i++) {
			sectBuff[i] = spiTransferByte(0xFF);
		}
		sectBuff[i] = 0;
		spiTransferByte(0xFF); // CRC lo byte
		spiTransferByte(0xFF); // CRC hi byte

		mmcSectorCount = mmcExtractSectorsFromCSD(sectBuff);

		#ifdef MMC_DEBUG
		rprintfStr("MMC CSD: 0x");
		i = 0;
		for(i = 0; i <0x10; i++) {
			rprintfu08(sectBuff[i]);
			rprintfChar(' ');
		}
		rprintfCRLF();
		rprintfu32(mmcSectorCount);
		rprintfNum(10,10, FALSE,' ', mmcSectorCount);
		rprintfCRLF();
		#endif
		rprintfu32(mmcSectorCount);
	}
	sbi(MMC_CS_PORT,MMC_CS_PIN);

	//		for(;;);
	//		sectBuff[0] = 'a';
	//		sectBuff[1] = 'b';
	//		sectBuff[2] = 'c';
	//		sectBuff[3] = 'd';
	//		sectBuff[4] = 'e';

	// 	i = -1;
	// 	while((0x0 != i) && (0x5 != i)) {
	// 		i = mmcWrite(0x40000, sectBuff);
	// 		_delay_ms(100);
	// 	}
	// 	memset(sectBuff, 0, sizeof(u08)*sectBuffSize);
	// 	while(0x0 != mmcRead(0x40000,sectBuff)) {
	// 		_delay_ms(100);
	// 	}
	// 
	// 	rprintfStr((char *)sectBuff);
	// 	rprintfCRLF();

	/**
	 * Setting
	 * ip:	10.0.0.42
	 * netm:	255.255.255.0
	 * gw:	10.0.0.1
	 */
	netstackInit(0xa000024,0xffffff00,0xa000001);
	// Interrupt from nic on falling edge
	sbi(EICRA,ISC01);
	cbi(EICRA,ISC00);
	// Enabling interrupt from nic
	cbi(DDRD,INT0);
	sbi(PORTD,INT0);
	sbi(EIMSK,INT0);
	sei();
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

ISR(INT0_vect,ISR_NOBLOCK) {
	rprintfStr("Got interrupt...\r\n");
	netstackService();
}
