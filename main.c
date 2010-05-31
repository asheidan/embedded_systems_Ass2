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

// Local headers
#include "lm74.h"
#include "mmcfunctions.h"
#include "temperature_timer.h"

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
u16 mmcSectoSize;

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
		mmcSectoSize = mmcExtractSectorSizeFromCSD(sectBuff);

		#ifdef MMC_DEBUG
		rprintfProgStrM("MMC CSD: 0x");
		i = 0;
		for(i = 0; i < 0x10; i++) {
			rprintfu08(sectBuff[i]);
			rprintfChar(' ');
		}
		rprintfCRLF();
		rprintfProgStrM("MMC Sectors: 0x");
		rprintfu32(mmcSectorCount);
		rprintfCRLF();
		rprintfProgStrM("MMC Sector size: 0x");
		rprintfu16(mmcSectoSize);
		rprintfCRLF();
		#endif
	}
	sbi(MMC_CS_PORT,MMC_CS_PIN);

	init_temperature_timer();

	for(;;) {
	}
}

