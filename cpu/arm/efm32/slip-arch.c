/**
* \file
*			Machine dependent EFM32 SLIP routines for UART0.
* \author
*			Jerome Cloute-Cazalaa
*/

#include <stdio.h>
#include "contiki.h"
#include "dev/slip.h"
#include "dev/uart0.h"

/*--------------------------------------------------------------------------*/
void
slip_arch_init(unsigned long ubr)
{ 
  uart0_set_input(slip_input_byte);
}
/*--------------------------------------------------------------------------*/
void
slip_arch_writeb(unsigned char c)
{
  uart0_writeb(c);

}
/** @} */