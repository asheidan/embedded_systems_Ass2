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
#include "mmcfunctions.h"

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

inline void nicRegDump(void) {}
const char bootMessage[]	PROGMEM = "Initialising...";

/*
void tcpSend(uint32_t dstIp, uint16_t dstPort, uint16_t len, uint8_t* data) {
	struct netTcpHeader* tcpHeader;

	// Risky business
	data -= TCP_HEADER_LEN;
	tcpHeader = (struct tcpHeader*)data;
	len += TCP_HEADER_LEN;

	tcpHeader->destport = HTONS(dstPort);
	tcpHeader->srcPort = HTONS(dstPort);
	tcpHeader->tcplen = htons(len);
	tcpHeader->tcpchksim = 0;

	ipSend(dstIp, IP_PROTO_TCP, len, data);
}
*/

#define sectBuffSize 512
u08 sectBuff[sectBuffSize];
u32 mmcSectorCount;
u16 mmcSectoSize;

void alive(void) {
	cbi(PORTA,PA0);
	_delay_ms(500);
	sbi(PORTA,PA0);
	_delay_ms(500);
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
	netstackInit(IPDOT(10l,0l,0l,42l),0xffffff00,0xa000001);
	// Interrupt from nic on falling edge
	sbi(EICRA,ISC01);
	cbi(EICRA,ISC00);
	// Enabling interrupt from nic
	cbi(DDRD,INT0);
	sbi(PORTD,INT0);
	sbi(EIMSK,INT0);
	sei();
	// enc28j60RegDump();
	for(;;) {
		temperature = read_temperature();
		format_temperature(tmpStringBuff, temperature);
		rprintfStr(tmpStringBuff);
		rprintfCRLF();
		i = strlen(tmpStringBuff);
		tmpStringBuff[i++] = '\r';
		tmpStringBuff[i++] = '\n';
		strcpy((char *)&sectBuff[ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN], tmpStringBuff);
		udpSend(IPDOT(10l,0l,0l,1l), 1234, i, &sectBuff[ETH_HEADER_LEN + IP_HEADER_LEN + UDP_HEADER_LEN]);
		// uart_transmit_byte(temperature >> 8);
		// uart_transmit_byte(temperature & 0xFF);
		// uart_transmit_string(newLine);
		_delay_ms(5000);
	}
}

#ifdef UART_USE_INT
ISR(INT0_vect,ISR_NOBLOCK) {
#else
ISR(INT0_vect) {
#endif
	//u08 r;
	// rprintfProgStrM("Got interrupt...\r\n");
	netstackService();
	// rprintfu08(r);
	// rprintfCRLF();
}
