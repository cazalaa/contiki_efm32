/**
 * \file
 *         Contiki main file
 * \author
 *         Jerome Cloute-Cazalaa <Silabs>
 */
 
#include "contiki.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/process.h>
#include <sys/procinit.h>
#include <sys/autostart.h>
#include <sys/energest.h>
#include <sys/rtimer.h>
#include <lib/sensors.h>
#include <clock.h>
#include <etimer.h>

#include <rimeaddr.h>
#include <net/netstack.h>

#include "dev/serial-line.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "net/netstack.h"
#include "net/mac/frame802154.h"

#include "core_cm3.h"

#include "platform-conf.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_common.h"

#include "power.h"
#include "usart0.h"
#include "usart2.h"
#include "leuart1.h"
#include "uart1.h"
#include "efm32flash.h"
#include "dmactrl.h"

#if WITH_UIP6
#include "net/uip-ds6.h"
#endif /* WITH_UIP6 */

#include "net/rime.h"

#include "sys/autostart.h"

#if UIP_CONF_ROUTER

#ifndef UIP_ROUTER_MODULE
#ifdef UIP_CONF_ROUTER_MODULE
#define UIP_ROUTER_MODULE UIP_CONF_ROUTER_MODULE
#else /* UIP_CONF_ROUTER_MODULE */
#define UIP_ROUTER_MODULE rimeroute
#endif /* UIP_CONF_ROUTER_MODULE */
#endif /* UIP_ROUTER_MODULE */

extern const struct uip_router UIP_ROUTER_MODULE;
#endif /* UIP_CONF_ROUTER */

#ifndef WITH_UIP
#define WITH_UIP 0
#endif

#if WITH_UIP
#include "net/uip.h"
#include "net/uip-fw.h"
#include "net/uip-fw-drv.h"
#include "net/uip-over-mesh.h"
static struct uip_fw_netif slipif =
  {UIP_FW_NETIF(192,168,1,2, 255,255,255,255, slip_send)};
static struct uip_fw_netif meshif =
  {UIP_FW_NETIF(172,16,0,0, 255,255,0,0, uip_over_mesh_send)};

#endif /* WITH_UIP */

#define UIP_OVER_MESH_CHANNEL 8
#if WITH_UIP
static uint8_t is_gateway;
#endif /* WITH_UIP */

#ifdef EXPERIMENT_SETUP
#include "experiment-setup.h"
#endif

void uip_log(char *msg) { puts(msg); }

#ifndef RF_CHANNEL
#define RF_CHANNEL              1
#endif

#ifndef NODE_ID
#define NODE_ID	0x02
#endif /* NODE_ID */

static unsigned short node_id=NODE_ID;

static void set_rime_addr(void) {
	rimeaddr_t n_addr;
	int i;

	memset(&n_addr, 0, sizeof(rimeaddr_t));

	// Set node address
#if UIP_CONF_IPV6
	n_addr.u8[7] = node_id & 0xff;
	n_addr.u8[6] = node_id >> 8;
#else
	n_addr.u8[0] = node_id & 0xff;
	n_addr.u8[1] = node_id >> 8;
#endif

	rimeaddr_set_node_addr(&n_addr);
	printf("Rime started with address ");
	for(i = 0; i < sizeof(n_addr.u8) - 1; i++) {
		printf("%d.", n_addr.u8[i]);
	}
	printf("%d\r\n", n_addr.u8[i]);
}

#if !PROCESS_CONF_NO_PROCESS_NAMES
static void print_processes(struct process * const processes[]) {
	/*  const struct process * const * p = processes;*/
	printf("Starting");
	while(*processes != NULL) {
		printf(" '%s'", (*processes)->name);
		processes++;
	}
	printf("\r\n");
}
#endif /* !PROCESS_CONF_NO_PROCESS_NAMES */

#if WITH_UIP
static void set_gateway(void) {
	if(!is_gateway) {
		//leds_on(LEDS_RED);
		//printf("%d.%d: making myself the IP network gateway.\n\n",
		//rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]);
		//printf("IPv4 address of the gateway: %d.%d.%d.%d\n\n",
		//uip_ipaddr_to_quad(&uip_hostaddr));
		
		/*
		uip_over_mesh_set_gateway(&rimeaddr_node_addr);
		uip_over_mesh_make_announced_gateway();
		is_gateway = 1;
		*/
	}
}
#endif /* WITH_UIP */


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
volatile uint32_t *reg;

int
main(void)
{
  rimeaddr_t addr;
  uint64_t uuid;

  uint8_t longaddr[8];
  uint16_t shortaddr;

  uint32_t u32_fullresetcause, u32_resetcause;

  CHIP_Init();
	
  //CMU_HFRCOBandSet(HFRCOBAND_VALUE);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  // Get Full Reset Cause as soon as possible
  u32_fullresetcause = RMU->RSTCAUSE;
  u32_resetcause = RMU_ResetCauseGet();

  // Get Main reset cause and clear it for next
  RMU_ResetCauseClear();

  power_init();

  setupSWO();
  
  burtc_crystal_init(u32_fullresetcause);

  gpio_config();
  watchdog_init();

  clock_init();
  dma_init();

  uuid = SYSTEM_GetUnique();
  efm32_flash_init();
   process_init();
  
#ifndef NO_RTIMER
    // Process Init
    rtimer_init();
    //printf("rtimer init\n\r");
#endif
  
  process_start(&etimer_process, NULL); 
  
  ctimer_init();
  
  button_sensor_init(); 

  /*
   * Serial Line Interface
   */

#ifdef DEBUGUART_USART0
  usart0_init(115200);
  usart0_set_input(serial_line_input_byte);
#elif DEBUGUART_LEUART1
  leuart1_init(9600);
  leuart1_set_input(serial_line_input_byte);
#elif DEBUGUART_UART0
  uart0_init(115200);
  uart0_set_input(serial_line_input_byte);
#elif DEBUGUART_UART1
  uart1_init(115200);
  uart1_set_input(serial_line_input_byte);
#elif DEBUGUART_CDC
#else
#warning "No Debug UART configured"
#endif



 // i2c_init(400);


  process_start(&sensors_process, NULL);
 
  serial_line_init();
  _spi1_config();

  si446x_init();

  
  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);
  //printf("\r\nenergest init done \r\n");
  //autostart_start(autostart_processes);
  //printf("\r\nautostart done \r\n");
  /*
   * This is the scheduler loop.
   */
  watchdog_start();
  node_id = NODE_ID;
  
  printf("\e[1;1H\e[2J"); // clear terminal if supported
  printf("\r\nWelcome to STK3700_RF \r\n");
  printf(CONTIKI_VERSION_STRING " started. ");
	if(node_id > 0) {
		printf("Node id is set to %u.\r\n", node_id);
	} else {
		printf("Node id is not set.\r\n");
	}
 	set_rime_addr();
#if WITH_UIP6
	memcpy(&uip_lladdr.addr, rimeaddr_node_addr.u8,
		UIP_LLADDR_LEN > RIMEADDR_SIZE ? RIMEADDR_SIZE : UIP_LLADDR_LEN);

/* Setup X-MAC for 802.15.4 */
	queuebuf_init();
	NETSTACK_RDC.init();
	NETSTACK_MAC.init();
	NETSTACK_NETWORK.init();

	printf("UIP6 %s %s, channel check rate %i Hz, radio channel %u\r\n",
		NETSTACK_MAC.name, NETSTACK_RDC.name,
		CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
		NETSTACK_RDC.channel_check_interval()),
		RF_CHANNEL);

	process_start(&tcpip_process, NULL);


	/*shortaddr = (rimeaddr_node_addr.u8[0] << 8) +
			rimeaddr_node_addr.u8[1];
	memset(longaddr, 0, sizeof(longaddr));
	rimeaddr_copy((rimeaddr_t *)&longaddr, &rimeaddr_node_addr);

	printf("MAC %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x \r\n",
			longaddr[0], longaddr[1], longaddr[2], longaddr[3],
			longaddr[4], longaddr[5], longaddr[6], longaddr[7]);*/
	printf("Tentative link-local IPv6 address ");
	{
		uip_ds6_addr_t *lladdr;
		int i;
		lladdr = uip_ds6_get_link_local(-1);
		for(i = 0; i < 7; ++i) {
			printf("%02x%02x:", lladdr->ipaddr.u8[i * 2],
				lladdr->ipaddr.u8[i * 2 + 1]);
		}
		printf("%02x%02x\r\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
	}

	if(!UIP_CONF_IPV6_RPL) {
		uip_ipaddr_t ipaddr;
		int i;
		uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
		uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
		uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);
		printf("Tentative global IPv6 address ");
		for(i = 0; i < 7; ++i) {
			printf("%02x%02x:",
				ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
		}
		printf("%02x%02x\n",
			ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
	}

#else /* WITH_UIP6 */

	NETSTACK_RDC.init();
	NETSTACK_MAC.init();
	NETSTACK_NETWORK.init();

	printf("UIP %s %s, channel check rate %i Hz, radio channel %u\r\n",
		NETSTACK_MAC.name, NETSTACK_RDC.name,
		CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0? 1:
		NETSTACK_RDC.channel_check_interval()),
		RF_CHANNEL);
#endif /* WITH_UIP6 */

#if !WITH_UIP && !WITH_UIP6
	//uart1_set_input(serial_line_input_byte);
	//serial_line_init();
#endif

#if PROFILE_CONF_ON
	profile_init();
#endif /* PROFILE_CONF_ON */

	leds_off(LEDS_GREEN);

#if TIMESYNCH_CONF_ENABLED
	timesynch_init();
	timesynch_set_authority_level((linkaddr_node_addr.u8[0] << 4) + 16);
#endif /* TIMESYNCH_CONF_ENABLED */

#if WITH_UIP
	process_start(&tcpip_process, NULL);
	process_start(&uip_fw_process, NULL);	/* Start IP output */
	process_start(&slip_process, NULL);

	slip_set_input_callback(set_gateway);

	{
		uip_ipaddr_t hostaddr, netmask;

		uip_init();

		uip_ipaddr(&hostaddr, 172,16,
			linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
		uip_ipaddr(&netmask, 255,255,0,0);
		uip_ipaddr_copy(&meshif.ipaddr, &hostaddr);

		uip_sethostaddr(&hostaddr);
		uip_setnetmask(&netmask);
		uip_over_mesh_set_net(&hostaddr, &netmask);
		/*    uip_fw_register(&slipif);*/
		uip_over_mesh_set_gateway_netif(&slipif);
		uip_fw_default(&meshif);
		uip_over_mesh_init(UIP_OVER_MESH_CHANNEL);
		printf("uIP started with IP address %d.%d.%d.%d\r\n",
			uip_ipaddr_to_quad(&hostaddr));
	}
#endif /* WITH_UIP */

	energest_init();
	ENERGEST_ON(ENERGEST_TYPE_CPU);

	watchdog_start();
	/* Stop the watchdog */
	watchdog_stop();

#if !PROCESS_CONF_NO_PROCESS_NAMES
	print_processes(autostart_processes);
#else /* !PROCESS_CONF_NO_PROCESS_NAMES */
	printf("\r\n"); /* include putchar() */
#endif /* !PROCESS_CONF_NO_PROCESS_NAMES */
	autostart_start(autostart_processes);
	/*
	 * This is the scheduler loop.
	 */
	for(;;) {
		do {
	        // Reset watchdog.
        	watchdog_periodic();
		} while(process_run() > 0);
		
		ENERGEST_OFF(ENERGEST_TYPE_CPU);
		watchdog_stop();    
		ENERGEST_ON(ENERGEST_TYPE_LPM);
		// Go to sleep until an interrupt occurs (and debug isn't enabled)
    	
#if DEBUG_BUILD != 1
		asm("wfi"::);
#endif
			
		// We are awake.
		watchdog_start();
		ENERGEST_OFF(ENERGEST_TYPE_LPM);
		ENERGEST_ON(ENERGEST_TYPE_CPU);
	}
	
	return 0;
}



