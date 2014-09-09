/**
 * \file
 *         A very simple test application for the stm32_bv motes
 * \author
 *         Benjamin Vedder
 */

#include "contiki.h"
#include "dev/leds.h"
//#include "dev/cc2520/cc2520.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(hello_world_process, ev, data)
{
	static struct etimer timer;
	
	PROCESS_BEGIN();

	printf("Hello, world\r\n");
	//cc2520_set_txpower(0x13);
	//printf("TX Power 0x%0X\r\n", cc2520_get_txpower());
	
	uint8_t reg;
	//CC2520_READ_RAM(&reg, CC2520_CHIPID, 1);
	//printf("Chip ID: 0x%02X\r\n", reg);
  
	/* Set a timer here. We will generate an event every times this timer expires
	 * etimer_set accepts clock ticks as its 2nd argument.
	 * CLOCK_CONF_SECOND is the number of ticks per second.
	 * This CLOCK_CONF_SECOND * N = N seconds */
	etimer_set(&timer, CLOCK_CONF_SECOND / 2);
  
	for (;;) {
		/* Wait on our timer */
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		
		leds_toggle(LEDS_GREEN);
		/* reset the timer so we can wait on it again */
		etimer_reset(&timer);
	}
  
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
