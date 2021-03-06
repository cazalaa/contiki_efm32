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
#define PACKETBUF_CONF_SIZE 128

/* NETSTACK_CONF_NETWORK specifies the network layer and can be either
   sicslowpan_driver, for IPv6 networking, or rime_driver, for the
   custom Rime network stack. */
/* Network Stack */
#if UIP_CONF_IPV6
#define NETSTACK_CONF_NETWORK sicslowpan_driver
#else
#define NETSTACK_CONF_NETWORK rime_driver
#endif

/* NETSTACK_CONF_MAC specifies the Medium Access Control (MAC)
   layer. The nullmac_driver does not provide any MAC
   functionality. The csma_driver is the default CSMA MAC layer, but
   is not compatible with all radios. */
#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC  nullmac_driver
//#define NETSTACK_CONF_MAC   csma_driver 
#endif /* NETSTACK_CONF_MAC */

#ifndef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC   nullrdc_driver
//#define NETSTACK_CONF_RDC   sicslowmac_driver
//#define NETSTACK_CONF_RDC   contikimac_driver
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


/* UIP_CONF_IPV6 specifies whether or not IPv6 should be used. If IPv6
   is not used, IPv4 is used instead. */
/*#ifndef UIP_CONF_IPV6
#define UIP_CONF_IPV6 1
#endif *//* UIP_CONF_IPV6 */

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
#define NULLRDC_CONF_802154_AUTOACK      1

// 
#if WITH_UIP6


/* Specify a minimum packet size for 6lowpan compression to be
   enabled. This is needed for ContikiMAC, which needs packets to be
   larger than a specified size, if no ContikiMAC header should be
   used. */
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD 63
#define CONTIKIMAC_CONF_WITH_CONTIKIMAC_HEADER 0

#define CXMAC_CONF_ANNOUNCEMENTS         0
#define XMAC_CONF_ANNOUNCEMENTS          0

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                8
#endif

#else /* WITH_UIP6 */

/* Network setup for non-IPv6 (rime). */

//#define NETSTACK_CONF_NETWORK rime_driver

#define COLLECT_CONF_ANNOUNCEMENTS       1
#define CXMAC_CONF_ANNOUNCEMENTS         0
#define XMAC_CONF_ANNOUNCEMENTS          0
#define CONTIKIMAC_CONF_ANNOUNCEMENTS    0

#define CONTIKIMAC_CONF_COMPOWER         1
#define XMAC_CONF_COMPOWER               1
#define CXMAC_CONF_COMPOWER              1

#ifndef COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS
#define COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS     32
#endif /* COLLECT_NEIGHBOR_CONF_MAX_COLLECT_NEIGHBORS */

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                16
#endif /* QUEUEBUF_CONF_NUM */

#ifndef TIMESYNCH_CONF_ENABLED
#define TIMESYNCH_CONF_ENABLED           0
#endif /* TIMESYNCH_CONF_ENABLED */

#endif /* WITH_UIP6 */

// 
#define PACKETBUF_CONF_ATTRS_INLINE 1

#ifndef RF_CHANNEL
#define RF_CHANNEL 1              
#endif /* RF_CHANNEL */

// 
#define CONTIKIMAC_CONF_BROADCAST_RATE_LIMIT 0
 

#define IEEE802154_CONF_PANID       0xABCD

#define SHELL_VARS_CONF_RAM_BEGIN 0x1100
#define SHELL_VARS_CONF_RAM_END 0x2000

#define PROFILE_CONF_ON 0
#define ENERGEST_CONF_ON 1

#define ELFLOADER_CONF_TEXT_IN_ROM 0
#ifndef ELFLOADER_CONF_DATAMEMORY_SIZE
#define ELFLOADER_CONF_DATAMEMORY_SIZE 0x400
#endif /* ELFLOADER_CONF_DATAMEMORY_SIZE */
#ifndef ELFLOADER_CONF_TEXTMEMORY_SIZE
#define ELFLOADER_CONF_TEXTMEMORY_SIZE 0x800
#endif /* ELFLOADER_CONF_TEXTMEMORY_SIZE */



#define AODV_COMPLIANCE
#define AODV_NUM_RT_ENTRIES 32

#define WITH_ASCII 1

#define PROCESS_CONF_NUMEVENTS 8
#define PROCESS_CONF_STATS 1

#define PROCESS_CONF_FASTPOLL    4


#ifdef WITH_UIP6
// 
#define RIMEADDR_CONF_SIZE              8

#define UIP_CONF_LL_802154              1
#define UIP_CONF_LLH_LEN                0

#define UIP_CONF_ROUTER                 1
#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL               1
#endif /* UIP_CONF_IPV6_RPL */

// 
/* configure number of neighbors and routes */
#ifndef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     30
#endif /* UIP_CONF_DS6_NBR_NBU */
#ifndef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   30
#endif /* UIP_CONF_DS6_ROUTE_NBU */

#define UIP_CONF_ND6_SEND_RA		0
#define UIP_CONF_ND6_REACHABLE_TIME     600000
#define UIP_CONF_ND6_RETRANS_TIMER      10000

#define UIP_CONF_IPV6                   1
#ifndef UIP_CONF_IPV6_QUEUE_PKT
#define UIP_CONF_IPV6_QUEUE_PKT         0
#endif /* UIP_CONF_IPV6_QUEUE_PKT */
#define UIP_CONF_IPV6_CHECKS            1
#define UIP_CONF_IPV6_REASSEMBLY        0
#define UIP_CONF_NETIF_MAX_ADDRESSES    3
#define UIP_CONF_ND6_MAX_PREFIXES       3
#define UIP_CONF_ND6_MAX_NEIGHBORS      4
#define UIP_CONF_ND6_MAX_DEFROUTERS     2
#define UIP_CONF_IP_FORWARD             0
#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE		1000
#endif

// 
#define SICSLOWPAN_CONF_COMPRESSION_IPV6        0
#define SICSLOWPAN_CONF_COMPRESSION_HC1         1
#define SICSLOWPAN_CONF_COMPRESSION_HC01        2
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_COMPRESSION_HC06
#ifndef SICSLOWPAN_CONF_FRAG
#define SICSLOWPAN_CONF_FRAG                    1
#define SICSLOWPAN_CONF_MAXAGE                  8

#endif /* SICSLOWPAN_CONF_FRAG */

#define SICSLOWPAN_CONF_CONVENTIONAL_MAC	1
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS       2
#ifndef SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS
#define SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS   5
#endif /* SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS */  
#else /* WITH_UIP6 */
#define UIP_CONF_IP_FORWARD      1
// 
#define UIP_CONF_BUFFER_SIZE     1000  

#endif /* WITH_UIP6 */  
// 
#define UIP_CONF_ICMP_DEST_UNREACH 1

#define UIP_CONF_DHCP_LIGHT
#define UIP_CONF_LLH_LEN         0
#ifndef  UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  48
#endif

#ifndef  UIP_CONF_TCP_MSS
#define UIP_CONF_TCP_MSS         200
#endif
#define UIP_CONF_MAX_CONNECTIONS 4
#define UIP_CONF_MAX_LISTENPORTS 8
#define UIP_CONF_UDP_CONNS       12
#define UIP_CONF_FWCACHE_SIZE    30
#define UIP_CONF_BROADCAST       1
#define UIP_ARCH_IPCHKSUM        1
#define UIP_CONF_UDP             1
#define UIP_CONF_UDP_CHECKSUMS   1
#define UIP_CONF_PINGADDRCONF    0
#define UIP_CONF_LOGGING         0

 

#endif /* CONTIKI_CONF_H */
