/**
* \file
*			uip-arch.c
* \author
*			jerome Cloute-Cazalaa
*/

#include "net/uip.h"
#include "net/uip_arch.h"

#if UIP_TCP

void  uip_add32(uint8_t * op32, uint16_t op16)
{
  uint32_t op32_align, uip_acc32_align;
  
  op32_align =
      ((uint32_t) op32[0]) << 24 | ((uint32_t) op32[1]) << 16 |
      ((uint16_t) op32[2]) << 8 | op32[3];

  uip_acc32_align = op32_align + op16;
  uip_acc32[3] = uip_acc32_align;
  uip_acc32[2] = uip_acc32_align >> 8;
  uip_acc32[1] = uip_acc32_align >> 16;
  uip_acc32[0] = uip_acc32_align >> 24;
}

#endif

/*-----------------------------------------------------------------------------------*/
/** @} */