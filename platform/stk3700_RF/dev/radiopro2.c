/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */
#include "platform-conf.h"
#include "radio_config.h"  


/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
//uint8_t Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;
//uint8_t Radio_Configuration_Data_Array[] = { 0x07, 0x02, 0x01, 0x00, 0x01, 0x8C, 0xBA, 0x80, 0x00 };
//uint8_t   *Radio_ConfigurationArray = &Radio_Configuration_Data_Array[0];
//tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;
//tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;
uint16_t  Radio_Delay_Cnt_After_Reset = RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET;
//uint8_t                        RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER;
//uint8_t                        RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH;
//uint8_t                        RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP;

//                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,                   \
//                            RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,              \
//                            RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,       \
//                            RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       \


/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  /* Setup SysTick Timer for 1 msec interrupts  */
 /*
 if (SysTick_Config(SystemCoreClock / 1000)) while (1) ;
   GPIO_PinOutToggle(PORT_LED1, PORT_PIN_LED1);
  uint32_t curTicks;
  GPIO_PinOutToggle(PORT_LED1, PORT_PIN_LED1);
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  */

  clock_wait(dlyTicks);

}

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);
/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  uint16_t wDelay = 0;
 // GPIO_PinOutToggle(PORT_LED0, PORT_PIN_LED0);

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  //GPIO_PinOutToggle(PORT_LED1, PORT_PIN_LED1);
  for (; wDelay < Radio_Delay_Cnt_After_Reset; wDelay++);
  //GPIO_PinOutToggle(PORT_LED0, PORT_PIN_LED0);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  /* Power Up the radio chip */
  vRadio_PowerUp();
       
	si446x_gpio_pin_cfg(0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00);
  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init())
  {
    /* Error hook */
    //BSP_LedSet(0);
    //Delay(2);
    //BSP_LedClear(0);
    /* Power Up the radio chip */
    vRadio_PowerUp();
  }
 
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
  
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void vRadio_StartTx(uint8_t channel, uint8_t *pioFixRadioPacket)
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Start sending packet on channel, START immediately, Packet according to PH */
  si446x_start_tx(channel, 0u, 0u);
}
