/*
 * DS18B20.h
 *
 *  Created on: Dec 6, 2014
 *      Author: Qizhang
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "onewire.h"

// DS18B20 ROM command set
#define SEARCH_ROM			0xF0
#define READ_ROM			0x33
#define MATCH_ROM			0x55
#define SKIP_ROM            0xCC
#define ALARM_SEARCH		0xEC

// DS18B20 function command set
#define CONVERT_T           0x44
#define READ_SCRATCHPAD    	0xBE
#define WRITE_SCRATCHPAD	0x4E
#define COPY_SCRATCHPAD		0x48
#define RECALL_E2			0xB8
#define READ_POWER_SUPPLY	0xB4

uint16_t DS1820ReadTempRegisters(void);
uint16_t DS1820GetTemp(void);
uint8_t DS1820TempSign(uint16_t temp);
uint8_t DS1820TempInteger(uint16_t temp);
uint8_t DS1820TempFraction(uint16_t temp);

#endif /* DS18B20_H_ */
