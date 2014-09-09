/**
 * \file
 *         Si446x Contiki Radio driver file
 * \author
 *         Jerome Cloute-Cazalaa <Silabs>
 */
 
 
#include "platform-conf.h"
#include "contiki.h"
#include "dev/leds.h"
#include "net/packetbuf.h"
#include "net/rime/rimestats.h"
#include "net/netstack.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif

//#define WITH_SEND_CCA 1

#define FOOTER_LEN 2
#define HEADER_LEN 2

//#define AUX_LEN (HEADER_LEN + FOOTER_LEN)
#define AUX_LEN 0

#define FOOTER1_CRC_OK      0x80
#define FOOTER1_CORRELATION 0x7f


int si446x_packets_seen, si446x_packets_read;
int16_t si446x_last_rssi;
uint8_t si446x_last_lqi;

PROCESS(Si446x_process, "Si446x driver");

/*---------------------------------------------------------------------------*/
int
si446x_init(void)
{
  uint16_t part_number ;
	uint8_t chip_rev ;

	vSpiInitialize();
  vRadio_Init();
  si446x_part_info();

	chip_rev =  Si446xCmd.PART_INFO.CHIPREV;
 
  leds_toggle(LEDS_ALL); 
  process_start(&Si446x_process, NULL);
  
  // enable NIRQ interrupt on the MCU
  GPIO_IntConfig(PORT_RF_NIRQ, PORT_PIN_RF_NIRQ, false, true, true);
  printf("radio init done\n\r");

  return 1;
}
/*---------------------------------------------------------------------------*/
  static int
si446x_prepare(const void *payload, unsigned short payload_len)
{
	uint8_t total_len;
	int i;
 
	PRINTF("si446x: sending %d bytes\n\r", payload_len);

	RIMESTATS_ADD(lltx);

	  PRINTF("payload Contents :\n\r");
	  for(i=0; i<payload_len; i++)
	  {
		  PRINTF("%02X ",((uint8_t *)payload)[i]);
		  if((i%16) == 15) PRINTF("\n\r");
	  }
	  PRINTF("\n\r");

	total_len = payload_len;
 si446x_write_tx_fifo(payload_len, payload);
  printf("prepare\n\r");
	return 0;
}

/*---------------------------------------------------------------------------*/
static int
si446x_transmit(unsigned short transmit_len)
{
  // Transmit on channel 0 only, immediatly, return to ready
  si446x_start_tx(0, 0x80, transmit_len  );
  printf("tramsmit\n\r");
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
si446x_send(const void *payload, unsigned short payload_len)
{
 si446x_prepare(payload, payload_len);
 printf("send\n\r");
 return si446x_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int
si446x_read(void *buf, unsigned short buf_len)
{ 
  uint8_t *buf1;
  buf1=(uint8_t *) (buf);    
  uint8_t header[HEADER_LEN];
  uint16_t len;
  si446x_fifo_info(0);
  len=Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
  
  
  //si446x_read_rx_fifo(HEADER_LEN, buf);  
  //len = (buf1[0]& 0x07)*256 + buf1[1];
  si446x_read_rx_fifo(len, buf);
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);  
  
//   if(len > SI466X_FIFO_SIZE) {
// 	/* Oops, we must be out of sync. */
// 	 //flushrx();
//   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
// 	return 0;
//   }
// 
//   if(len <= AUX_LEN) {
// 	//flushrx();
//   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
//   return 0;
//   } 
// 
//   if(len - AUX_LEN > buf_len) {
// 	//flushrx();
//   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT); 
//   return 0;
//   } 
// 
// 
//   /* Check RX Fifo Error */
//    //if( status() == CC112X_STATE_RXFIFO_ERROR )
//   si446x_get_int_status(0u, 0u, 0u);
//   if (Si446xCmd.GET_INT_STATUS.CHIP_STATUS & SI446X_CMD_GET_INT_STATUS_REP_FIFO_UNDERFLOW_OVERFLOW_ERROR_BIT)
//   {
// 	  //flushrx();
//     si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT); 
// 	  return 0;
//   }

 //flushrx();
     si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
     si446x_get_int_status(0u, 0u, 0u); 
     printf("read\n\r");
     //printf("packet received\n\r");
  return (int) len;
}

/*---------------------------------------------------------------------------*/
static int
si446x_channel_clear(void)
{
printf("cca\n\r");
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
si446x_receiving_packet(void)
{
printf("packet receiving\n\r");
  //si446x_get_int_status(0u, 0u, 0u);
  //return (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_MASK);
  return radio_hal_NirqLevel();
}
/*---------------------------------------------------------------------------*/
static int
si446x_pending_packet(void)
{
  si446x_get_int_status(0u, 0u, 0u);
  printf("packet pending\n\r");
  return (Si446xCmd.GET_INT_STATUS.PH_PEND && SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_MASK);

}
/*---------------------------------------------------------------------------*/
static int
si446x_on(void)
{
  si446x_get_int_status(0u, 0u, 0u);
  si446x_start_rx(0u, 0u, 0u,
         SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE, 
         SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
         SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_NOCHANGE
         );
  printf("radio on\n\r");
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
si446x_off(void)
{
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
  printf("radio off\n\r");
  return 1;
}
/*---------------------------------------------------------------------------*/
const struct radio_driver si446x_radio_driver =
  {
    si446x_init,
    si446x_prepare,
    si446x_transmit,
    si446x_send,
    si446x_read,
    si446x_channel_clear,
    si446x_receiving_packet,
    si446x_pending_packet,
    si446x_on,
    si446x_off,
  };
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * \brief Forward received packets to upper layer.
 */
PROCESS_THREAD(Si446x_process, ev, data)
{
  int len;

  PROCESS_BEGIN();

  printf("Si446x_process: started\n");
  

  while(1) {
	 PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
   //printf("Si446x_process: begin\n");  
   si446x_get_int_status(0u, 0u, 0u);
      packetbuf_clear();
      len = si446x_read(packetbuf_dataptr(), PACKETBUF_SIZE);

      packetbuf_set_datalen(len);

      NETSTACK_RDC.input();
  
   //printf("Si446x_process: len=%d , %s\n",len,packetbuf_dataptr() );
   }

  PROCESS_END();
}
