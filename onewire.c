/*
 * 1-Wire implementation for MSP430
 * Modified from David Siroky's <siroky@dasir.cz> original onewire library
 * All operation timings are based on Maxim Application Note 126: 1-Wire Communication Through Software
 * @license: MIT
 */
#include <msp430.h>
#include <stdint.h>
#include "onewire.h"
#include "delay.h"

/***************************************************************/


void ow_portsetup(void) {
	OWPORTDIR |= OWPORTPIN;
	OWPORTOUT |= OWPORTPIN;
	// Typical pull-up/down resistor from MSP430 is 35kOhm, too large for the ideal value ~5kOhm
	OWPORTREN |= OWPORTPIN;
}

/***************************************************************/

uint8_t onewire_reset(void)
{
	volatile uint8_t echo = 1;
	// Standard mode
	OW_LO	// Drive bus low, delay 480us
	DELAY_US(480);
	OW_RLS	// Release bus, delay 70us
	DELAY_US(70);
	if (OWPORTIN & OWPORTPIN) echo = 0;	// Sample bus, 0 = device(s) present, 1 = no device present
	DELAY_US(410); 		// Delay 410us
	return echo;		// Report device presence
}

//#####################################################################

void onewire_write_bit(uint8_t bit)
{
	OW_HI
	if (bit) {	// Write 1
		OW_LO	// Drive bus low, delay 6us
		DELAY_US(6);
		OW_RLS	// Release bus, delay 64us
		DELAY_US(64);
	}
	else {	//Write 0
		OW_LO	// Drive bus low, delay 60us
		DELAY_US(60);
		OW_RLS	// Release bus, delay 10us
		DELAY_US(10);
	}
 }

//#####################################################################

uint8_t onewire_read_bit(void)
{
	volatile uint8_t bit = 0;
	OW_LO	// Drive bus low, delay 6us
	DELAY_US(6);
	OW_RLS	// Release bus, delay 9us
	DELAY_US(9);
	if (OWPORTIN & OWPORTPIN) bit = 1; // Sample bus to read bit from slave
	DELAY_US(55); // delay 55us
	return bit;
}

//#####################################################################

void onewire_write_byte(uint8_t byte)
{
	volatile uint8_t i;
	for(i = 0; i < 8; i++)
	{
		onewire_write_bit(byte & 1); // LSB first
		byte >>= 1;
	}
}

//#####################################################################

uint8_t onewire_read_byte(void)
{
	volatile uint8_t i;
	volatile uint8_t byte = 0;
	for(i = 0; i < 8; i++)
	{
		byte >>= 1;
		if (onewire_read_bit()) byte |= 0x80;
	}
	return byte;
}
