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
#include "dev/button-sensor.h"

#include <stdio.h> /* For printf() */

static struct etimer et_radio;
uint8_t tx_buf[]={0x00,0x0F,0x68,0x65,0x6C,0x6C,0x6F,0x68,0x65,0x6C,0x6C,0x6F, 0x68,0x65,0x6C,0x6C,0x6F};
uint8_t buf[256];
uint8_t temp; 
int i;


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS(radio_test_process, "Radio test process");
PROCESS(button_test_process, "Button test process");
AUTOSTART_PROCESSES(&radio_test_process, &button_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(radio_test_process, ev, data)
{
  PROCESS_BEGIN();
    
  printf("\n\rTest Radio\n\r"); 
  NETSTACK_RADIO.on();
  printf("radio in RX\r\n");
    
    
  while(1) {
  

     PROCESS_YIELD();
    
    if(NETSTACK_RADIO.pending_packet())
    {
       temp = NETSTACK_RADIO.read(buf, 256); 
      printf ("len = %d\r\n", temp); 
       for (i=0; i< temp; i++)        
        { printf (" %2x,", buf[i]);}
        printf ("%s\r\n", buf);  
        NETSTACK_RADIO.on();      
        //leds_toggle(LEDS_ALL);
    }
    
   
  }
  
  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
//PROCESS(button_test_process, "Button test process");
//AUTOSTART_PROCESSES(&button_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(button_test_process, ev, data)
{
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_up_sensor);
  SENSORS_ACTIVATE(button_down_sensor);

  while(1) {
    PROCESS_YIELD();

    if(ev == sensors_event) {
      if(data == &button_down_sensor) {
        NETSTACK_RADIO.send(tx_buf,sizeof(tx_buf));
        printf("radio in TX\r\n");
        leds_toggle(LEDS_ALL);
        NETSTACK_RADIO.on();
        printf("radio in RX\r\n");
      } else if(data == &button_up_sensor) {
        leds_toggle(LEDS_ALL);
        GPIO_PinOutClear(PORT_LED0, PORT_PIN_LED0);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
