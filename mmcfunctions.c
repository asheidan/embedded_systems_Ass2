
#include "mmcfunctions.h"
#include "rprintf.h"

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
