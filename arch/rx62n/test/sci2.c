#include "sci2.h"
#include <iodefine.h>
#include <board.h>

#define SCI2_BAUDRATE	115200

void sci2_init (void)
{
	MSTP(SCI2) = 0;					    /* Enable module */
	SCI2.SCR.BYTE = 0;				    /* Reset module */
	IOPORT.PFFSCI.BIT.SCI2S = 1;			    /* Remap pins to B set */

	PORT5.DDR.BIT.B2 = 0;				    /* Make RX input */
	PORT5.ICR.BIT.B2 = 1;				    /* Enable input buffer on RX pin */
	PORT5.DDR.BIT.B0 = 1;				    /* Make TX output */

	SCI2.SCR.BIT.CKE = 0;				    /* Use internal baudrate generator, SCK pin function os IO port */
	SCI2.SMR.BYTE = 0;				    /* PCLK/1, 8N1, async mode, multiprocessor mode disabled */
	SCI2.SCMR.BIT.SMIF = 0;				    /* Not smart card mode */
	SCI2.SCMR.BIT.SINV = 0;				    /* No TDR inversion */
	SCI2.SCMR.BIT.SDIR = 0;				    /* LSB first */
	SCI2.SEMR.BIT.ACS0 = 0;				    /* Use external clock */
	SCI2.SEMR.BIT.ABCS = 1;				    /* 8 base clock cycles for 1 bit period */
	/* Set baudrate */
	/* For 16 base clock cycles change formula to PCLK / (32 * BAUD - 1) */
	SCI2.BRR = PCLK_FREQUENCY / (16 * SCI2_BAUDRATE) - 1;
	/* Reset interrupt flags */
	IR(SCI2, TXI2) = 0;
	IR(SCI2, RXI2) = 0;
	/* Set priorities */
	IPR(SCI2, TXI2) = 15;
	IPR(SCI2, RXI2) = 15;
	/* Disable interrupts */
	IEN(SCI2, RXI2) = 0;
	IEN(SCI2, TXI2) = 0;
	IEN(SCI2, ERI2) = 0;
	IEN(SCI2, TEI2) = 0;
	for (int i = 20000ul; i > 0; --i) asm volatile ("nop");  /* Wait at least one bit interval */
	SCI2.SCR.BYTE |= 0x30; //enable tx/rx
	
	SCI2.SCR.BIT.TE = 1;				    /* Enable transmitter */
	SCI2.SCR.BIT.TIE = 1;				    /* Enable TX interrupt flag */
	SCI2.SCR.BIT.RE = 1;				    /* Enable RX interrupt flag */
	SCI2.SCR.BIT.RIE = 1;				    /* Enable RX interrupt flag */
	for (int i = 200000ul; i > 0; --i) asm volatile ("nop");  /* Wait at least one bit interval */
	IR(SCI2, TXI2) = 1;
	IR(SCI2, RXI2) = 0;//0 is polling
	
	SCI2.SCR.BYTE |= 0x00; //enable tx/rx
	SCI2.SCR.BYTE |= 0x30; //enable tx/rx
}

int sci2_putchar (int c)
{
	while (0 == SCI2.SSR.BIT.TEND);

	IR(SCI2, TXI2) = 0;
	SCI2.TDR = c;
	return (unsigned char)c;
}

int sci2_puts (const char *s)
{
	int ret = 0;
	while ('\0' != *s)
	{
		sci2_putchar(*s);
		++ret;
		++s;
	}
	return ret;
}

