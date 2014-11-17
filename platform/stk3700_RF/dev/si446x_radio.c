#include "platform-conf.h"
#include "contiki.h"
#include "dev/leds.h"
#include "net/packetbuf.h"
#include "net/rime/rimestats.h"
#include "net/netstack.h"

#ifndef DEBUG
#define DEBUG 0
#endif

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
static int
si446x_on(void)
{
  si446x_get_int_status(0u, 0u, 0u);
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT|SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
  si446x_start_rx(1u, 0u, 0u,
          SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );
#if DEBUG
       //  printf("on\n\r");
#endif
  return 1;
}
/*---------------------------------------------------------------------------*/
int
si446x_init(void)
{
  uint16_t part_number ;
	uint8_t chip_rev ;
      //GPIO_PinOutSet(PORT_LED0, PORT_PIN_LED1);
	vSpiInitialize();
  vRadio_Init();
  si446x_part_info();
	
	//part_number =  Si446xCmd.PART_INFO.PART;
	chip_rev =  Si446xCmd.PART_INFO.CHIPREV;
	
  //PRINTF("Starting Si446x Driver on chip id 0x%X rev 0x%X\n\r",
		  /*getreg(CC112X_PARTNUMBER), getreg(CC112X_PARTVERSION)*/ 
		//	part_number, chip_rev ); 
  leds_toggle(LEDS_ALL); 
  process_start(&Si446x_process, NULL);
  //si446x_start_tx(0, 0x30, 0  );
#if DEBUG
  printf("Si446x_init\n\r");
#endif
  GPIO_IntConfig(PORT_RF_NIRQ, PORT_PIN_RF_NIRQ, false, true, true);

  return 1;
}
/*---------------------------------------------------------------------------*/
  static int
si446x_prepare(const void *payload, unsigned short payload_len)
{
	int i;
	uint8_t buf1[2];

	RIMESTATS_ADD(lltx);
 	#if DEBUG
	  PRINTF("> %d \n\r", payload_len);
	  PRINTF("payload Contents : \n\r");
	  for(i=0; i<payload_len; i++)
	  {
		  PRINTF("%02X ",((uint8_t *)payload)[i]);
		  if((i%16) == 15) PRINTF("\n\r");
	  }
	  PRINTF("\n\r");	
	#endif
	buf1[0]=0x00;  // PHR --> MS=0 R1-R0=00 FCS=0 DW=0
	buf1[1]=payload_len;
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT|SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
 	si446x_write_tx_fifo(2, (uint8_t *) &buf1);  // load packet length
 	si446x_write_tx_fifo(payload_len, payload);
	#if DEBUG
 		//printf("prepare\n\r");
	#endif
  
	return 0;
}

/*---------------------------------------------------------------------------*/
static int
si446x_transmit(unsigned short transmit_len)
{
  // Transmit on channel 0 only, immediatly, return to ready
  si446x_start_tx(1, 0x30, transmit_len+2  );  // add 1 byte for packet length
#if DEBUG
  //printf("transmit --> \n\r");
#endif
  si446x_get_int_status(0u, 0u, 0u); 
  while (!(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_MASK))
  {
      si446x_get_int_status(0u, 0u, 0u); 
     
  }
#if DEBUG
  //printf("transmited\n\r");
#endif
  si446x_on(); //start RX
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
si446x_send(const void *payload, unsigned short payload_len)
{
 si446x_prepare(payload, payload_len);
 return si446x_transmit(payload_len);
#if DEBUG
 //printf("send\n\r");
#endif
}
/*---------------------------------------------------------------------------*/
static int
si446x_read(void *buf, unsigned short buf_len)
{ 
  uint8_t buf1[2];
  int i;
  //buf1=(uint8_t *) (buf);    
  //uint8_t header[HEADER_LEN];
  uint8_t len;
  si446x_fifo_info(0);
  //si446x_read_rx_fifo(1,(uint8_t *) &len); //read packet length
  si446x_read_rx_fifo(2,(uint8_t *) &buf1); //read packet length
  //len=Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;
 #if DEBUG 
  //printf("packet received lenght %d\n\r",len);
 #endif
   
  len = buf1[1];
  si446x_read_rx_fifo(len,(uint8_t *) buf); //read packet content
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT|SI446X_CMD_FIFO_INFO_ARG_RX_BIT);  //flush FIFOs for next packet 
 /*for(i=0; i<len; i++)
	  {
		  PRINTF("%02X ",((uint8_t *)buf)[i]);
		  if((i%16) == 15) PRINTF("\n\r");
	  }
	  PRINTF("\n\r");*/
  //si446x_on(); //start new RX
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
// 
//  //flushrx();
//      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
//      si446x_get_int_status(0u, 0u, 0u); 
#if DEBUG
      //printf("packet received\n\r");
#endif
  return (int) len;
}

/*---------------------------------------------------------------------------*/
static int
si446x_channel_clear(void)
{
#if DEBUG
  //printf("cca\n\r");
#endif
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
si446x_receiving_packet(void)
{
#if DEBUG
//printf("packet receiving\n\r");
#endif
  //si446x_get_int_status(0u, 0u, 0u);
  //return (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_MASK);
  return !radio_hal_NirqLevel();
}
/*---------------------------------------------------------------------------*/
static int
si446x_pending_packet(void)
{
  si446x_get_int_status(0u, 0u, 0u);
#if DEBUG
  //printf("packet pending?\n\r");
#endif
  return (Si446xCmd.GET_INT_STATUS.PH_PEND && SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_MASK);

}

/*---------------------------------------------------------------------------*/
static int
si446x_off(void)
{
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
#if DEBUG
  printf("off\n\r");
#endif
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
  int len,i;
  uint8_t *datar;

  PROCESS_BEGIN();
#if DEBUG
  printf("Si446x_process: started\n");
#endif 

  while(1) {
	 PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
#if DEBUG
   //printf("Si446x_process: begin\n");  
#endif
   si446x_get_int_status(0u, 0u, 0u);
      packetbuf_clear();
      len = si446x_read(packetbuf_dataptr(), PACKETBUF_SIZE);
	//len = si446x_read(&datar, PACKETBUF_SIZE);
      

#if DEBUG  
   printf("<%d :\n",len );
   //memcpy(packetbuf_dataptr(),&datar[0],len);
   datar=packetbuf_dataptr();
   PRINTF("Received Payload Contents :\n\r");
   for(i=0; i<len; i++)
	  {
		  PRINTF("%02X ",((uint8_t *)datar)[i]);
		  if((i%16) == 15) PRINTF("\n\r");
	  }
	  PRINTF("\n\r");
#endif
   packetbuf_set_datalen(len);
   NETSTACK_RDC.input();
  si446x_on(); //start new RX
   }

	
  PROCESS_END();
}
