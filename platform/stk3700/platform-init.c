
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


void setupSWO(void)
{
	/* Enable GPIO Clock. */
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
	/* Enable Serial wire output pin */
	GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;
#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY) || defined(_EFM32_LEOPARD_FAMILY)
	/* Set location 0 */
	GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;

	/* Enable output on pin - GPIO Port F, Pin 2 */
	GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
	GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
#else
	/* Set location 1 */
	GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC1;
	/* Enable output on pin */
	GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
	GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
#endif
	/* Enable debug clock AUXHFRCO */
	CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

	while(!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY));

	/* Enable trace in core debug */
	CoreDebug->DHCSR |= 1;
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* Enable PC and IRQ sampling output */
	DWT->CTRL = 0x400113FF;
	/* Set TPIU prescaler to 16. */
	TPI->ACPR = 0xf;
	/* Set protocol to NRZ */
	TPI->SPPR = 2;
	/* Disable continuous formatting */
	TPI->FFCR = 0x100;
	/* Unlock ITM and output data */
	ITM->LAR = 0xC5ACCE55;
	ITM->TCR = 0x10009;
}


void _gpio_config(void)
{
  /*
   * Mux Gpios
   */

  // Leds
  GPIO_PinModeSet(gpioPortE,2,gpioModePushPull,0);
  GPIO_PinModeSet(gpioPortE,3,gpioModePushPull,0);

  // USART 0
  //GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1); // TX
  //GPIO_PinModeSet(gpioPortE, 11, gpioModeInput, 0);    // RX
  gpio_set_output_pushpull(GPIO_USART0_TX,1);
  //gpio_set_output_pushpull(GPIO_USART0_RX,1);
  gpio_set_input(GPIO_USART0_RX);


  // ITM SWO Enable (loc 0 - PF2)
  GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 0);
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;

  // I2C0
  gpio_set_mode(GPIO_I2C0_SDA, gpioModeWiredAndDrivePullUp , 1);
  gpio_set_mode(GPIO_I2C0_SCL, gpioModeWiredAndDrivePullUp, 1);
  //gpio_set_output(GPIO_I2C0_SDA,1);
  //gpio_set_output(GPIO_I2C0_SCL,1);
  //gpio_set_output_pushpull(GPIO_I2C0_SDA,1);
  //gpio_set_output_pushpull(GPIO_I2C0_SCL,1);
  //GPIO_PinModeSet(gpioPortC, 6, gpioModeWiredAndPullUp, 1);
  //GPIO_PinModeSet(gpioPortC, 7, gpioModeWiredAndPullUp, 1);

#ifdef WITH_SPI2
  // SPI2
  gpio_set_output_pushpull(GPIO_USART2_TX,0);
  gpio_set_input(GPIO_USART2_RX);
  //gpio_set_mode(GPIO_USART2_RX, gpioModeInputPull , 1);
  gpio_set_output_pushpull(GPIO_USART2_CLK,0);
  gpio_set_output_pushpull(GPIO_USART2_CS,1);
#endif

#ifdef WITH_SPI1
  gpio_set_output_pushpull(GPIO_USART1_TX,0);
  gpio_set_input(GPIO_USART1_RX);
  //gpio_set_mode(GPIO_USART1_RX, gpioModeInputPull , 1);
  gpio_set_output_pushpull(GPIO_USART1_CLK,0);
  gpio_set_output_pushpull(GPIO_USART1_CS,1);
#endif

#ifdef WITH_LEUART1
  gpio_set_output_pushpull(GPIO_LEUART1_TX,0);
  gpio_set_input(GPIO_LEUART1_RX);
#endif

#ifdef WITH_UART0
  gpio_set_output_pushpull(GPIO_UART0_TX,0);
  gpio_set_input(GPIO_UART0_RX);
#endif

#ifdef WITH_UART1
  gpio_set_output_pushpull(GPIO_UART1_TX,0);
  gpio_set_input(GPIO_UART1_RX);
#endif

  leds_arch_init();
}


