/**
 * \file
 *         platform initialization file
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
#include <clock.h>
#include <etimer.h>

#include <rimeaddr.h>
#include <net/netstack.h>

#include "dev/serial-line.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/button-sensor.h"
#include "lib/sensors.h"

#include "core_cm3.h"

#include "platform-conf.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "efm32gg990f1024.h"

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

static struct timer debouncetimer;
SENSORS(&button_up_sensor, &button_down_sensor);
PROCESS_NAME(Si446x_process);

// debug trace 
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


void gpio_config(void)
{
  // Enable Port Clock to access registers
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);


  
  /*
   * Mux Gpios
   */
  /* Buttons */
	GPIO_PinModeSet(BUTTON_CONF_UP_PORT, BUTTON_CONF_UP_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BUTTON_CONF_DOWN_PORT, BUTTON_CONF_DOWN_PIN, gpioModeInput, 0);


  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(BUTTON_CONF_UP_PORT, BUTTON_CONF_UP_PIN, false, true, true);
  GPIO_IntConfig(BUTTON_CONF_DOWN_PORT, BUTTON_CONF_DOWN_PIN, false, true, true);
  
  // Leds
  GPIO_PinModeSet(gpioPortE,2,gpioModePushPull,0);
  GPIO_PinModeSet(gpioPortE,3,gpioModePushPull,0);
  
  //RF IRQ
  GPIO_PinModeSet(PORT_RF_NIRQ, PORT_PIN_RF_NIRQ, gpioModeInput, 0);
  

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

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
  /* RF PWDN */  // Si446x
GPIO_PinModeSet(PORT_RF_PWRDN, PORT_PIN_RF_PWRDN, PORT_MODE_RF_PWRDN, 0);    

  leds_arch_init();
}


void _spi1_config(void)
{
 USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  /* Enable module clocks */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_HFPER, true);

  /* Reset USART just in case */
  USART_Reset(USART_USED);

  /* Enable clock for USART1 */
  CMU_ClockEnable(USART_CLK, true);

  /* Configure to use SPI master with manual CS */
  /* For now, configure SPI for worst case 14MHz clock in order to work for all */
  /* configurations. */
   init.refFreq  = 14000000;
   init.baudrate = 1000000;
   init.msbf     = true;
  USART_InitSync(USART_USED, &init);  
  
  /* Module USART1 is configured to location 1 */
  USART_USED->ROUTE = (USART_USED->ROUTE & ~_USART_ROUTE_LOCATION_MASK) |
                      USART_ROUTE_LOCATION;

  /* Enable signals TX, RX, CLK, CS */
  USART_USED->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN |
                       USART_ROUTE_CLKPEN;
}

/*---------------------------------------------------------------------------*/
/**
 * \brief GPIO interrupt hanlder
 */
 

void
GPIO_ODD_IRQHandler(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
 
  if(!timer_expired(&debouncetimer)) {
    return;
  }
  timer_set(&debouncetimer, CLOCK_SECOND / 8);

  if(GPIO->IF & (1UL << BUTTON_CONF_UP_PIN)) 
  {
    GPIO->IFC = (1UL << BUTTON_CONF_UP_PIN);
    sensors_changed(&button_up_sensor);
  } 
  if(GPIO->IF & (1UL << BUTTON_CONF_DOWN_PIN)) {
    GPIO->IFC = (1UL << BUTTON_CONF_DOWN_PIN);
    sensors_changed(&button_down_sensor); 
  }
  if(GPIO->IF & (1UL << PORT_PIN_RF_NIRQ)) {
    GPIO->IFC = (1UL << PORT_PIN_RF_NIRQ);
    // If RX packet, wake up thread
    si446x_get_int_status(0u, 0u, 0u);
    GPIO->IFC = (1UL << PORT_PIN_RF_NIRQ);
		process_poll(&Si446x_process); 
  }
   
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief GPIO interrupt hanlder
 */


 
void   
GPIO_EVEN_IRQHandler(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  
  if(!timer_expired(&debouncetimer)) {
    return;
  }
  timer_set(&debouncetimer, CLOCK_SECOND / 8);

  if(GPIO->IF & (1UL << BUTTON_CONF_UP_PIN)) 
  {
    GPIO->IFC = (1UL << BUTTON_CONF_UP_PIN);
    sensors_changed(&button_up_sensor);
  } 
  if(GPIO->IF & (1UL << BUTTON_CONF_DOWN_PIN)) {
    GPIO->IFC = (1UL << BUTTON_CONF_DOWN_PIN);
    sensors_changed(&button_down_sensor); 
  } 
  if(GPIO->IF & (1UL << PORT_PIN_RF_NIRQ)) {
    
    // If RX packet, wake up thread
    
    si446x_get_int_status(0u, 0u, 0u);
    GPIO->IFC = (1UL << PORT_PIN_RF_NIRQ);
		process_poll(&Si446x_process); 
    }

  
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/*---------------------------------------------------------------------------*/
void
button_sensor_init()
{
  /* Init debounce timer */
  timer_set(&debouncetimer, 0);

}

static int
config_up()
{
  
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the change button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_down()
{
  
  return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_up_sensor, BUTTON_SENSOR, NULL, config_up, NULL);
SENSORS_SENSOR(button_down_sensor, BUTTON_SENSOR, NULL, config_down, NULL);

