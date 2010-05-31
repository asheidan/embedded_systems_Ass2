#ifndef _MMCFUNCTIONS_H_
#define _MMCFUNCTIONS_H_

#include "global.h"

#define CONFIG_SECTOR 0
#define START_SECTOR (CONFIG_SECTOR + 1)

typedef struct config {
	u08 version_number;
	u32 current_sector;
	u08 check_sum;
} config;

extern volatile char mmc_buf[512];
extern volatile char config_buf[512];
extern volatile config *conf;
extern volatile int mmc_buf_i;

u32 mmcExtractSectorsFromCSD(u08 *buff);
u16 mmcExtractSectorSizeFromCSD(u08 *buff);

void mmcFormatCard(void);
void mmcInitCard(void);

#endif
