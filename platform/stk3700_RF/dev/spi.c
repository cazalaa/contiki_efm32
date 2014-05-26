/*!
 * File:
 *  spi.c
 *  author: Shaoxian Luo
 *
 * Description:
 *  This file is the driver of the spi which is used for the communication with the radio
 *
 * Silicon Laboratories Confidential
 * Copyright 2013 Silicon Laboratories, Inc.
 */

#include "platform-conf.h"
 
/*!
 * This function is used to initialize the SPI port.
 *
 *  @return None
 */
void vSpiInitialize(void)
{
/*  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(USART_CLK, true);
   USART_Reset(USART_USED);
  
  init.baudrate     = 5000000;
  init.databits     = usartDatabits8;
  init.msbf         = 1;
  init.master       = 1;
  init.clockMode    = usartClockMode0;
  init.prsRxEnable  = 0;
  init.autoTx       = 0;

  USART_InitSync(USART1, &init);
 */   
 
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
  /* IO configuration (USART 1, Location #1) */
  //GPIO_PinModeSet(SPI_MOSI_PORT, SPI_MOSI_PIN, gpioModePushPull, 1);
  //GPIO_PinModeSet(PORT_RF_SPI_MISO, PORT_PIN_RF_SPI_MISO, gpioModeInput, 0);
  //GPIO_PinModeSet(SPI_CLK_PORT, SPI_CLK_PIN, gpioModePushPull, 0);
  //GPIO_PinModeSet(SPI_CS_PORT, SPI_CS_PIN, gpioModePushPull, 1);
}

/*!
* This function is used to read/write one byte from/to SPI port (target: EzRadioPRO).
 *
 * @param[in] biDataIn    Data to be sent.
 * @return  Read value of the SPI port after writing on it.
 */   
uint8_t bSpiReadWrite(uint8_t biDataIn)
{  
  USART_TypeDef *spi = USART1;
 
  spi->TXDATA = biDataIn;
  while (!(spi->STATUS & USART_STATUS_TXC))
  {
  }
  return (uint8_t)(spi->RXDATA);
}

/*!
 * This function is used to send data over SPI port (target: EzRadioPRO).no response expected.
 *
 *  @param[in] biDataInLength  The length of the data.
 *  @param[in] *pabiDataIn     Pointer to the first element of the data.
 *
 *  @return None
 */
void vSpiWriteDataBurst(uint8_t biDataInLength, uint8_t *pabiDataIn)
{
  while (biDataInLength--) {
    bSpiReadWrite(*pabiDataIn++);
  }
}

/*!
 * This function is used to read data from SPI port.(target: EzRadioPRO).
 *
 *  \param[in] biDataOutLength  The length of the data.
 *  \param[out] *paboDataOut    Pointer to the first element of the response.
 *
 *  \return None
 */
void vSpiReadDataBurst(uint8_t biDataOutLength, uint8_t *paboDataOut)
{
  // send command and get response from the radio IC
  while (biDataOutLength--) {
    *paboDataOut++ = bSpiReadWrite(0xFF);
  }
}

/**
 *  Pull down nSEL of the selected device.
 *
 *  @param[in] qiSelect Device select.
 *
 *  @note Input: qiSelect <br> 0 - \b DUT <br> 1 - \b LCD <br>
 *
 ******************************************************************************/
void vSpi_ClearNsel(eSpi_Nsel qiSelect)
{
  switch (qiSelect)
  {
      case eSpi_Nsel_RF:
        GPIO_PinOutClear(PORT_RF_NSEL, PORT_PIN_RF_NSEL);
        break;
      default:
        break;
  }
}

/**
 *  Pull-up nSEL of the selected device.
 *
 *  @param[in] qiSelect Device select.
 *
 *  @note Input: qiSelect <br> 0 - \b DUT <br> 1 - \b LCD <br>
 *
 ******************************************************************************/
void vSpi_SetNsel(eSpi_Nsel qiSelect)
{
  switch (qiSelect)
  {
      case eSpi_Nsel_RF:
        GPIO_PinOutSet(PORT_RF_NSEL, PORT_PIN_RF_NSEL);
        break;
      default:
        break;
  }
}
