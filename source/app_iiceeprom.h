#ifndef _IIC_EEPROM_H
#define _IIC_EEPROM_H

#include "app_eepromnew.h"

EEprom* EEprom_newIicEEprom(const char* devPath, unsigned int pageSize, unsigned int pageCount, const char* name);

#endif