
#include "contiki.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/process.h>
#include <sys/procinit.h>
#include <sys/autostart.h>
#include <sys/energest.h>
#include <sys/rtimer.h>
#include <clock.h>
#include <etimer.h>

#include <rimeaddr.h>
#include <net/netstack.h>

#include "dev/serial-line.h"
#include "dev/leds.h"
#include "dev/watchdog.h"

#include "core_cm3.h"

#include "platform-conf.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_gpio.h"

#include "power.h"
#include "usart0.h"
#include "usart2.h"
#include "leuart1.h"
#include "uart1.h"
#include "efm32flash.h"
#include "dmactrl.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

unsigned short node_id;
unsigned char node_mac[8];
unsigned int idle_count = 0;

/*--------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

void MemManage_Handler(void)
{
  while(1);
}

void BusFault_Handler(void)
{
  while(1);
}
void UsageFault_Handler(void)
{
  while(1);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
volatile uint32_t *reg;

int
main(void)
{
  rimeaddr_t addr;
  uint64_t uuid;

  uint32_t u32_fullresetcause, u32_resetcause;

  CHIP_Init();

  CMU_HFRCOBandSet(HFRCOBAND_VALUE);

  // Get Full Reset Cause as soon as possible
  u32_fullresetcause = RMU->RSTCAUSE;
  u32_resetcause = RMU_ResetCauseGet();

  // Get Main reset cause and clear it for next
  RMU_ResetCauseClear();

  power_init();

  setupSWO();
  
  burtc_crystal_init(u32_fullresetcause);



  gpio_init();
  _gpio_config();
  watchdog_init();

  clock_init();
  dma_init();

  uuid = SYSTEM_GetUnique();
  efm32_flash_init();

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
#else
#warning "No Debug UART configured"
#endif

#ifndef NO_RTIMER
    // Process Init
    rtimer_init();
#endif
  printf("\e[1;1H\e[2J");
  printf("\r\nWelcome to STK3700 \r\n");

  process_init();
  process_start(&etimer_process, NULL);

  serial_line_init();

  // Initializing nodeid, MAC
  node_id = uuid & 0xFFFF;

  memset(&addr, 0, sizeof(rimeaddr_t));
  memset(node_mac, 0, sizeof(node_mac));
  node_mac[7] = 0x02;
  node_mac[6] = 0x4B;
  node_mac[5] = 0x06;
  node_mac[1] = node_id >> 8;
  node_mac[0] = node_id & 0xff;

  addr.u8[0] = node_id & 0xff;
  addr.u8[1] = node_id >> 8;
  rimeaddr_set_node_addr(&addr);

#ifndef NO_NETWORK
    // Radio Chip init
    //NETSTACK_RADIO.init();

    /*
     * Network Init
     */
    //NETSTACK_RDC.init();
    NETSTACK_MAC.init();
    NETSTACK_NETWORK.init();

#endif /* NO_NETWORK */


  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  autostart_start(autostart_processes);

  /*
   * This is the scheduler loop.
   */
  watchdog_start();
#ifdef PLATFORM_DEBUG
  watchdog_stop(); /* Stop the wdt... */
#endif
  while(1) {
    int r;
    do {
        /* Reset watchdog. */
        //watchdog_periodic();
        r = process_run();
    } while(r > 0);

    /*
    * Idle processing.
    */
    idle_count++;
    /* Stop processor clock */
    //__WFI(); // Make Standby freeze at wakeup ... maybe use RT task for enter_standby ?

#if 0 // From MSP430
  int s = splhigh();        /* Disable interrupts. */
  if (process_nevents() != 0) {
    splx(s);            /* Re-enable interrupts. */
  } else {
      .... go to sleep !
  }
#endif
      //
    __NOP();
  }

  return 0;
}




