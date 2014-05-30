/**
 * \file
 *         Si446x radio test   TX "hello" payload every second
 * \author
 *         Jerome Cloute-Cazalaa
 */

#include "contiki.h"
#include "platform-conf.h"
#include "net/netstack.h"
#include "dev/leds.h"

#include <stdio.h> /* For printf() */

static struct etimer et_radio;

/*---------------------------------------------------------------------------*/
PROCESS(radio_test_process, "Radio test process");
AUTOSTART_PROCESSES(&radio_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(radio_test_process, ev, data)
{
  PROCESS_BEGIN();

  printf("\n\rTest Radio\n\r");
  
  while(1) {
    etimer_set(&et_radio, CLOCK_SECOND );
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    NETSTACK_RADIO.send("hello",5);
    leds_toggle(LEDS_ALL);
    etimer_set(&et_radio, CLOCK_SECOND );
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    si446x_change_state(1);
   
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
