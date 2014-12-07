#include <msp430.h>
#include <stdint.h>
#include "DS18B20.h"

uint8_t Rx_Data = '0';
uint8_t ERROR_TEXT[] = "Invalid command.\n";

void uartInit(void) {
	/* Config TX P2.0, and RX P2.1 */
	P2SEL1 |= BIT0 + BIT1;
	P2SEL0 &= ~(BIT0 + BIT1);
	/* Set up eUSCI_A for UART */
	UCA0CTLW0 |= UCSWRST;
	UCA0CTLW0 |= UCSSEL__SMCLK;
	// Recommended setting when fBRCLK = 1MHz, BR = 9600
	UCA0BRW = 6;
	UCA0MCTLW |= UCBRS5 + UCBRF3 + UCOS16;
	UCA0CTLW0 &= ~UCSWRST;
	UCA0IE |= UCRXIE;	// Enable RX interrupt
}

void uartTransferByte(uint8_t data) {
	while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
	UCA0TXBUF = data;
}

void uartTransferText(uint8_t * text) {
	volatile uint8_t i;
	for(i = 0; text[i] != '\0'; i++) {
		uartTransferByte(text[i]);
	}
}

void DS1820TempSerialPrint(uint16_t temp) {
	volatile uint8_t integer = DS1820TempInteger(temp);
	if (DS1820TempSign(temp)) uartTransferByte('-');
	else uartTransferByte('+');
	uartTransferByte(integer / 100 + 48);
	uartTransferByte(integer % 100 / 10 + 48);
	uartTransferByte(integer % 10 + 48);
	uartTransferByte('.');
	if (DS1820TempFraction(temp)) uartTransferByte('5');
	else uartTransferByte('0');
	uartTransferByte('\n');

}

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    /* Clock system configuration */
    CSCTL0 = CSKEY;				// Enable CS register access with correct password
    CSCTL1 = DCOFSEL_3;			// Set DCO frequency at 8MHz
    CSCTL2 = SELA__DCOCLK + SELS__DCOCLK + SELM__DCOCLK;	// Assign DCO as MCLK, SMCLK and ALK source
    CSCTL3 = DIVA__32 + DIVS__8 + DIVM__1;	// MCLK 8MHz, SMCLK 1MHz, ALK 250KHz

    P3OUT = 0;
    P3DIR |= BIT4 + BIT5 + BIT6;

    uartInit();
	__bis_SR_register(LPM0_bits + GIE);
    while(1) {
    	switch (Rx_Data) {
    		case '1':
    			P3OUT |= BIT6;	// Light a LED when reading temperature info from DS18B20
    			DS1820TempSerialPrint(DS1820GetTemp());
    			P3OUT &= ~BIT6;
    			break;

    		case '2':
    			P3OUT ^= BIT6;
    			break;

    		case '3':
    			P3OUT ^= BIT5;
    			break;

    		case '4':
    			P3OUT ^= BIT4;
    			break;

    		default:
    			uartTransferText(ERROR_TEXT);
    			break;
    	}
    	__bis_SR_register(LPM0_bits);			// Enter LPM0, interrupts enabled
    }
}

// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV, 0x08))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    Rx_Data = UCA0RXBUF;
    __bic_SR_register_on_exit(LPM0_bits);
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}

