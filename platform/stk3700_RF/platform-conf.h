/*
 */

/**
 * \file
 *         platform configuration file
 * \author
 *         Jerome Cloute-Cazalaa <Silabs>
 */

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

#include <stdint.h>
#include "efm32gg990f1024.h"
#include "gpio.h"

#ifdef WITH_SPI1
#include "usart1.h"
#endif

// Default systick config : 1ms by tick
#define CLOCK_CONF_SECOND 1000

/*
 * High Frequencies clock configuration
 */

// Default RCO clock is 14 MHz
#define HFRCOBAND_VALUE _CMU_HFRCOCTRL_BAND_14MHZ
#define HFRCO_FREQ (14*MHZ)

#define HFRXO_FREQ (48*MHZ)

// Use HFRCO as HFCLK
#define HFCLK_FREQ HFRXO_FREQ

// No divider for peripheral clock
#define HFPERCLK_DIV  1
#define HFPERCLK_FREQ (HFCLK_FREQ / HFPERCLK_DIV)

#define RTC_PRESCALE 32768

// To set Also in Application linker script
#define USERAPP_START_ADDR 0x10000


#define SERIAL_LINE_CONF_LOCALECHO
#define SHELL_CONF_PROMPT
#define SERIAL_SHELL_CONF_PROMPT(str) \
  printf("%02X%02X: %s",rimeaddr_node_addr.u8[1],rimeaddr_node_addr.u8[0],str)

//#define DEBUGUART_USART0 1
//#define DEBUGUART_LEUART1 1
#define DEBUGUART_UART0 1
//#define DEBUGUART_CDC 1

#if DEBUGUART_ITM
#include "core_cm3.h"
#define default_writeb     ITM_SendChar 

#elif DEBUGUART_CDC
#define defaultuart_send_bytes   CDC_send_bytes
#define defaultuart_writeb     CDC_WriteChar

#elif DEBUGUART_USART0
#define defaultuart_send_bytes   usart0_send_bytes
#define defaultuart_writeb       usart0_writeb
#define defaultuart_drain        usart0_drain
#define defaultuart_set_input    usart0_set_input
#define defaultuart_get_input    usart0_get_input

#elif DEBUGUART_UART0
#define defaultuart_send_bytes   uart0_send_bytes
#define defaultuart_writeb       uart0_writeb
#define defaultuart_drain        uart0_drain
#define defaultuart_set_input    uart0_set_input
#define defaultuart_get_input    uart0_get_input
#elif DEBUGUART_UART1
#define defaultuart_send_bytes   uart1_send_bytes
#define defaultuart_writeb       uart1_writeb
#define defaultuart_drain        uart1_drain
#define defaultuart_set_input    uart1_set_input
#define defaultuart_get_input    uart1_get_input

#elif DEBUGUART_LEUART1
#define defaultuart_send_bytes   leuart1_send_bytes
#define defaultuart_writeb       leuart1_writeb
#define defaultuart_drain        leuart1_drain
#define defaultuart_set_input    leuart1_set_input
#define defaultuart_get_input    leuart1_get_input

#endif

// Set USART0 as Debug UART
#define USART0_LOCATION  0
#if (USART0_LOCATION == 0)
#define GPIO_USART0_TX   GPIO_PE10
#define GPIO_USART0_RX   GPIO_PE11
#endif

#define USART1_LOCATION 1
#if (USART1_LOCATION == 1)
#define GPIO_USART1_TX  GPIO_PD0
#define GPIO_USART1_RX  GPIO_PD1
#define GPIO_USART1_CLK GPIO_PD2
#define GPIO_USART1_CS  GPIO_PD3
#else
#error "Unknown Location for USART1"
#endif

#define USART2_LOCATION 0
#if (USART2_LOCATION == 0)
#define GPIO_USART2_TX   GPIO_PC2
#define GPIO_USART2_RX   GPIO_PC3
#define GPIO_USART2_CLK  GPIO_PC4
#define GPIO_USART2_CS   GPIO_PC5
#else
#error "Unknown Location for USART2"
#endif

#define I2C0_LOCATION 3
#if (I2C0_LOCATION == 2)
#define GPIO_I2C0_SDA   GPIO_PC6
#define GPIO_I2C0_SCL   GPIO_PC7
#elif (I2C0_LOCATION == 3)
#define GPIO_I2C0_SDA   GPIO_PD14
#define GPIO_I2C0_SCL   GPIO_PD15
#endif

#define LEUART1_LOCATION 0
#if (LEUART1_LOCATION == 0)
#define GPIO_LEUART1_TX   GPIO_PC6
#define GPIO_LEUART1_RX   GPIO_PC7
#else
#error "Unknown Location for LEUART1"
#endif

#define UART0_LOCATION 1
#if (UART0_LOCATION == 1)
#define GPIO_UART0_TX   GPIO_PE0
#define GPIO_UART0_RX   GPIO_PE1
#else
#error "Unknown Location for UART0"
#endif

#define UART1_LOCATION 2
#if (UART1_LOCATION == 1)
#define GPIO_UART1_TX   GPIO_PF10
#define GPIO_UART1_RX   GPIO_PF11
#elif (UART1_LOCATION == 2)
#define GPIO_UART1_TX   GPIO_PB9
#define GPIO_UART1_RX   GPIO_PB10
#else
#error "Unknown Location for UART1"
#endif

#define GPIO_USER_LED1 GPIO_PE2
#define GPIO_USER_LED2 GPIO_PE3

#define PORT_LED0       gpioPortE
#define PORT_PIN_LED0   2
#define PORT_LED1       gpioPortE
#define PORT_PIN_LED1   3

/* Buttons ports STK3700 */
#define BUTTON_CONF_UP_PORT             gpioPortB
#define BUTTON_CONF_UP_PIN              9
#define BUTTON_CONF_DOWN_PORT           gpioPortB
#define BUTTON_CONF_DOWN_PIN            10

#define GPIO_BOOTLOADER GPIO_PD7

#define SPI_ENABLE()     gpio_set_value(GPIO_USART1_CS, 0)
#define SPI_DISABLE()    gpio_set_value(GPIO_USART1_CS, 1)
#define SPI_IS_ENABLED() (!gpio_get_value(GPIO_USART1_CS))

#define SPI_INIT()              spi1_init()
#define SPI_READWRITE(val)      spi1_readwrite(val)
#define SPI_READBUF(ptab,len)   spi1_readbuf(ptab,len)
#define SPI_WRITEBUF(ptab,len)  spi1_writebuf(ptab,len)

#define LEDS_CONF_ALL 3
#define LEDS_USER1    1
#define LEDS_USER2    2
#define LEDS_USER     LEDS_USER1


//RADIOPRO2

/* USART used for SPI access */
#define USART_USED        USART1
#define USART_CLK         cmuClock_USART1
#define USART_ROUTE_LOCATION USART_ROUTE_LOCATION_LOC1



/* USART read/write cycles */
#define USART_READ     1
#define USART_WRITE    0


#define MOSI_LOW()      GPIO->P[3].DOUT &= ~(1 << 0);
#define MOSI_HIGH()     GPIO->P[3].DOUT |= (1 << 0);

#define SCK_LOW()       GPIO->P[3].DOUT &= ~(1 << 2);
#define SCK_HIGH()      GPIO->P[3].DOUT |= (1 << 2);

#define RESET_LOW()     GPIO->P[3].DOUT &= ~(1 << 4);
#define RESET_HIGH()    GPIO->P[3].DOUT |= (1 << 4);

#define PORT_PIN_RF_PWRDN   	3
#define PORT_RF_PWRDN    			gpioPortC
#define PORT_MODE_RF_PWRDN		gpioModePushPull
#define SDN_LOW()							GPIO_PinOutClear(PORT_RF_PWRDN,PORT_PIN_RF_PWRDN)
#define SDN_HIGH()						GPIO_PinOutSet(PORT_RF_PWRDN,PORT_PIN_RF_PWRDN)

#define PORT_PIN_RF_SPI_SCK		0
#define PORT_RF_SPI_SCK				gpioPortD
#define PORT_MODE_RF_SPI_SCK	gpioModePushPull

#define PORT_PIN_RF_SPI_MISO	1
#define PORT_RF_SPI_MISO			gpioPortD
#define PORT_MODE_RF_SPI_MISO	gpioModeInput

#define PORT_PIN_RF_SPI_MOSI	2
#define PORT_RF_SPI_MOSI			gpioPortD
#define PORT_MODE_RF_SPI_MOSI	gpioModePushPull

#define PORT_PIN_RF_NSEL			3
#define PORT_RF_NSEL					gpioPortD
#define PORT_MODE_RF_NSEL			gpioModePushPull
#define CS_LOW()        GPIO_PinOutClear(PORT_RF_NSEL, PORT_PIN_RF_NSEL);
#define CS_HIGH()       GPIO_PinOutSet(PORT_RF_NSEL, PORT_PIN_RF_NSEL);

#define PORT_PIN_RF_SYNC_WORD_DETECT			5
#define PORT_RF_SYNC_WORD_DETECT 					gpioPortD
#define PORT_MODE_RF_SYNC_WORD_DETECT 		gpioModeInput

#if (	PORT_PIN_RF_SYNC_WORD_DETECT & 0x5555)
	#define PIN_RF_SYNC_WORD_IRQ GPIO_ODD_IRQn
	#define PIN_RF_SYNC_WORD_IRQHandler GPIO_ODD_IRQHandler
#else
	#define PIN_RF_SYNC_WORD_IRQ GPIO_EVEN_IRQn
	#define PIN_RF_SYNC_WORD_IRQHandler GPIO_EVEN_IRQHandler
#endif


#define PORT_PIN_RF_NIRQ    	0
#define PORT_RF_NIRQ    			gpioPortC
#define PORT_MODE_RF_NIRQ			gpioModeInput

#define PORT_PIN_RF_TX_DATA_CLK		6
#define PORT_RF_TX_DATA_CLK				gpioPortD
#define PORT_MODE_RF_TX_DATA_CLK	gpioModeInput


#define SILABS_RADIO_SI446X
#undef  SILABS_RADIO_SI4455


#define UIP_ARCH_ADD32                  1
#define UIP_ARCH_CHKSUM                 0
#define UIP_CONF_BYTE_ORDER             UIP_LITTLE_ENDIAN

/*! Extended driver support
 * Known issues: Some of the example projects
 * might not build with some extended drivers
 * due to data memory overflow */

#define RADIO_DRIVER_EXTENDED_SUPPORT

#include "si446x_api_lib.h"
#include "si446x_defs.h"
#include "si446x_nirq.h"
#include "radio_comm.h"
#include "radio_hal.h"
#include "si446x_radio.h"
#include "radiopro2.h"
#include "spi.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "stdarg.h" 
#include "platform-init.h"   




#endif /* __PLATFORM_CONF_H__ */
