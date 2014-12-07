#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <msp430.h>
#include <stdint.h>

// The pins connected with LEDs are not suitalbe for 1-wire port
// LED operations may interference the normal 1-wire protocal implentation
#define OWPORTDIR P2DIR
#define OWPORTOUT P2OUT
#define OWPORTIN P2IN
#define OWPORTREN P2REN
#define OWPORTPIN BIT5

// MSP430 GPIO pull-up/down resistor is ~35KOhm, too large to use
// The extra pull-down resistor on the device end is essential
#define OW_LO {	OWPORTDIR |= OWPORTPIN;	OWPORTREN &= ~OWPORTPIN; OWPORTOUT &= ~OWPORTPIN; }
#define OW_HI {	OWPORTDIR |= OWPORTPIN;	OWPORTREN &= ~OWPORTPIN; OWPORTOUT |= OWPORTPIN; }
#define OW_RLS { OWPORTDIR &= ~OWPORTPIN; OWPORTREN |= OWPORTPIN; OWPORTOUT |= OWPORTPIN; }

//########################################################################

void ow_portsetup(void);
uint8_t onewire_reset(void);
void onewire_write_bit(uint8_t bit);
uint8_t onewire_read_bit(void);
void onewire_write_byte(uint8_t byte);
uint8_t onewire_read_byte(void);

#endif /* ONEWIRE_H_ */
