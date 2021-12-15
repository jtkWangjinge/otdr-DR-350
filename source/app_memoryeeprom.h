#ifndef _APP_MEMORY_EEPROM_H
#define _APP_MEMORY_EEPROM_H

#include "app_eepromnew.h"

EEprom* EEprom_newMemoryEEprom(unsigned int pageSize, unsigned pageCount, const char* name);

#endif
