#ifndef _MMCFUNCTIONS_H_
#define _MMCFUNCTIONS_H_

#include "global.h"

u32 mmcExtractSectorsFromCSD(u08 *buff);
u16 mmcExtractSectorSizeFromCSD(u08 *buff);

#endif
