/**
 * \file
 *         Contiki configuration file
 * \author
 *         Jerome Cloute-Cazalaa <Silabs>
 */
 
#ifndef CONTIKI_CONF_H
#define CONTIKI_CONF_H

#include "platform-conf.h"

#define CC_CONF_INLINE inline
#define CC_CONF_DOUBLE_HASH 1
#define CCIF
#define CLIF

/* Types for clocks and uip_stats */
typedef unsigned short uip_stats_t;
typedef unsigned long clock_time_t;

#define KHZ 1000uL
#define MHZ 1000000uL

#define sleep(sec)   clock_wait(sec*1000)
#define msleep(msec) clock_wait(msec)
#define usleep(usec) clock_delay_usec(usec)

void clock_delay_msec(uint16_t dt);



#define SERIAL_LINE_CONF_LOCALECHO
#define SHELL_CONF_PROMPT
#define SERIAL_SHELL_CONF_PROMPT(str) \
  printf("%02X%02X: %s",rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0],str)



// Manchester encoded packets can use 128*2 sized packets
// Took into account in RF driver (use PACKETBUF_SIZE * 2)
//#define PACKETBUF_CONF_SIZE 256

/* NETSTACK_CONF_NETWORK specifies the network layer and can be either
   sicslowpan_driver, for IPv6 networking, or rime_driver, for the
   custom Rime network stack. */
#ifndef NETSTACK_CONF_NETWORK
//#define NETSTACK_CONF_NETWORK rime_driver
#define NETSTACK_CONF_NETWORK sicslowpan_driver 
#endif /* NETSTACK_CONF_NETWORK */

/* NETSTACK_CONF_MAC specifies the Medium Access Control (MAC)
   layer. The nullmac_driver does not provide any MAC
   functionality. The csma_driver is the default CSMA MAC layer, but
   is not compatible with all radios. */
#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC   nullmac_driver
//#define NETSTACK_CONF_MAC   csma_driver 
#endif /* NETSTACK_CONF_MAC */

#ifndef NETSTACK_CONF_RDC
//#define NETSTACK_CONF_RDC   nullrdc_driver
//#define NETSTACK_CONF_RDC   sicslowmac_driver
#define NETSTACK_CONF_RDC   contikimac_driver
//#define NETSTACK_CONF_RDC     nullrdc_noframer_driver
#endif /* NETSTACK_CONF_RDC */

#ifndef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154
//#define NETSTACK_CONF_FRAMER  framer_nullmac
#endif /* NETSTACK_CONF_FRAMER */

#ifndef NETSTACK_CONF_RADIO
//#define NETSTACK_CONF_RADIO   cc112x_driver
#define NETSTACK_CONF_RADIO   si446x_radio_driver
#endif /* NETSTACK_CONF_RADIO */

/* NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE specifies the channel check
   rate of the RDC layer. This defines how often the RDC will wake up
   and check for radio channel activity. A higher check rate results
   in higher communication performance at the cost of a higher power
   consumption. */
#ifndef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8
#endif /* NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE */

/* SICSLOWPAN_CONF_COMPRESSION specifies what 6lowpan compression
   mechanism to be used. 6lowpan hc06 is the default in Contiki. */
#ifndef SICSLOWPAN_CONF_COMPRESSION
//#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_HC06
//#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_HC1
#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_IPV6
#endif /* SICSLOWPAN_CONF_COMPRESSION */

/* UIP_CONF_IPV6 specifies whether or not IPv6 should be used. If IPv6
   is not used, IPv4 is used instead. */
#ifndef UIP_CONF_IPV6
#define UIP_CONF_IPV6 1
#endif /* UIP_CONF_IPV6 */

/* CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION specifies if ContikiMAC
   should optimize for the phase of neighbors. The phase optimization
   may reduce power consumption but is not compatible with all timer
   settings and is therefore off by default. */
#ifndef CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION
#define CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION 0
#endif /* CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION */

/* UIP_CONF_UDP specifies if UDP support should be included or
   not. Disabling UDP saves memory but breaks a lot of stuff. */
#ifndef UIP_CONF_UDP
#define UIP_CONF_UDP 1
#endif /* UIP_CONF_UDP */

/* UIP_CONF_TCP_SPLIT enables a performance optimization hack, where
   each maximum-sized TCP segment is split into two, to avoid the
   performance degradation that is caused by delayed ACKs. */
#ifndef UIP_CONF_TCP_SPLIT
#define UIP_CONF_TCP_SPLIT 1
#endif /* UIP_CONF_TCP_SPLIT */




#endif /* CONTIKI_CONF_H */
