/*
 * DS18B20.c
 *
 *  Created on: Dec 6, 2014
 *      Author: Qizhang
 */
#include "delay.h"
#include "DS18B20.h"

uint16_t DS1820ReadTempRegisters(void)
{
	volatile uint8_t i;
	volatile uint16_t bytes = 0;
	for(i = 16; i > 0; i--){
		bytes >>= 1;
		if (onewire_read_bit()) {
			bytes |= 0x8000;	// LSB first
		}
  }
  return bytes;
}


uint16_t DS1820GetTemp(void)
{
	volatile uint16_t temp;
	onewire_reset();
	onewire_write_byte(SKIP_ROM); // skip ROM command
	onewire_write_byte(CONVERT_T); // convert T command
	OW_HI
	DELAY_MS(750); // at least 750 ms for the default 12-bit resolution
	onewire_reset();
	onewire_write_byte(SKIP_ROM); // skip ROM command
	onewire_write_byte(READ_SCRATCHPAD); // read scratchpad command
	temp = DS1820ReadTempRegisters();
	return temp;
}

uint8_t DS1820TempSign(uint16_t temp)
{
	// Bit 12-15 are all identical: sign bit, so only need to read one: bit 15
	if (temp & 0x8000) return 1;
	return 0;
}

uint8_t DS1820TempInteger(uint16_t temp)
{
	//
	return (temp & 0x07F0) >> 4;
}

uint8_t DS1820TempFraction(uint16_t temp)
{
	return (temp & 0x0008) >> 3;
}
