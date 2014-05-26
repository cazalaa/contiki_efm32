/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2013 Silicon Laboratories, Inc.
 */

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */
#include "platform-conf.h"





                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

                /* ======================================= *
                 *     P U B L I C   F U N C T I O N S     *
                 * ======================================= */

void radio_hal_AssertShutdown(void)
{
    GPIO_PinOutSet(PORT_RF_PWRDN, PORT_PIN_RF_PWRDN);
}

void radio_hal_DeassertShutdown(void)
{
    GPIO_PinOutClear(PORT_RF_PWRDN, PORT_PIN_RF_PWRDN);
}

void radio_hal_ClearNsel(void)
{
    GPIO_PinOutClear(PORT_RF_NSEL, PORT_PIN_RF_NSEL);
}

void radio_hal_SetNsel(void)
{
    GPIO_PinOutSet(PORT_RF_NSEL, PORT_PIN_RF_NSEL);
}

uint8_t radio_hal_NirqLevel(void)
{
    return GPIO_PinInGet(PORT_RF_NIRQ, PORT_PIN_RF_NIRQ);
}

void radio_hal_SpiWriteByte(uint8_t byteToWrite)
{
  bSpiReadWrite(byteToWrite);
}

uint8_t radio_hal_SpiReadByte(void)
{
  return bSpiReadWrite(0xFF);
}

void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
  vSpiWriteDataBurst(byteCount, pData);
}

void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
  vSpiReadDataBurst(byteCount, pData);
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
/*uint8_t radio_hal_Gpio0Level(void)
{
    return GPIO_PinInGet(RF_GPIO0_PORT, RF_GPIO0_PIN);
}

uint8_t radio_hal_Gpio1Level(void)
{
    return GPIO_PinInGet(RF_GPIO1_PORT, RF_GPIO1_PIN);
}

uint8_t radio_hal_Gpio2Level(void)
{
    return GPIO_PinInGet(RF_GPIO2_PORT, RF_GPIO2_PIN);
}

uint8_t radio_hal_Gpio3Level(void)
{
    return GPIO_PinInGet(RF_GPIO3_PORT, RF_GPIO3_PIN);
}   
*/

#endif
