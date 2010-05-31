
#include "mmcfunctions.h"
#include "mmc.h"
#include "rprintf.h"

volatile char mmc_buf[512];
volatile char config_buf[512];
volatile config *conf;
volatile int mmc_buf_i;

u32 mmcExtractSectorsFromCSD(u08 *buff) {
	u16 c_size = 0;
	u08 c_mult = 0;
	c_size |= buff[0x06] & 0x03;
	c_size <<= 8;
	c_size |= buff[0x07];
	c_size <<= 2;
	c_size |= buff[0x08] >> 6;

	#ifdef MMC_DEBUG
	rprintfu16(c_size);
	rprintfCRLF();
	#endif

	c_mult |= buff[0x09] & 0x03;
	c_mult <<= 1;
	c_mult |= buff[0x0A] >> 7;

	#ifdef MMC_DEBUG
	rprintfu08(c_mult);
	rprintfCRLF();
	#endif

	return (u32)(c_size + 1) * ((u32)1 << (c_mult + 2));
}

u16 mmcExtractSectorSizeFromCSD(u08 *buff) {
	return (u16)1<<(buff[0x05] & 0x0F);
}

u08 checksum(u32 data) {
	u08 *d = (u08 *)&data;
	return d[0] + d[1] + d[2] + d[3] + VERSION_NUMBER;
}

void mmcInitCard(void) {
	mmcRead(CONFIG_SECTOR, config_buf);
	conf = (config *)config_buf;
	if (
			(VERSION_NUMBER == conf->version_number) &&
			(checksum(conf->current_sector) == conf->check_sum) &&
			(conf->current_sector < mmcSectorCount)
			) {
		// Config validates, use end sector.
		rprintfProgStrM("TIMER Old card...\r\n");
		rprintfProgStrM("TIMER Current sector: 0x");
		rprintfu32(conf->current_sector);
		rprintfCRLF();
		mmcRead(conf->current_sector, mmc_buf);
		mmc_buf_i = strlen(mmc_buf);
	} else {
		// Start anew.
		rprintfProgStrM("TIMER New card...\r\n");
		mmcFormatCard();
	}
}
void mmcFormatCard(void) {
	conf->current_sector = START_SECTOR;
	conf->version_number = VERSION_NUMBER;
	conf->check_sum = checksum(conf->current_sector);
	mmcWrite(CONFIG_SECTOR, config_buf);
	mmc_buf_i = 0;
}
