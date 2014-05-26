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
#define HEADER_LEN 0

//#define AUX_LEN (HEADER_LEN + FOOTER_LEN)
#define AUX_LEN 0

#define FOOTER1_CRC_OK      0x80
#define FOOTER1_CORRELATION 0x7f


int si446x_packets_seen, si446x_packets_read;
int16_t si446x_last_rssi;
uint8_t si446x_last_lqi;

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
  //si446x_start_tx(0, 0x30, 0  );
  return 1;
}
/*---------------------------------------------------------------------------*/
  static int
si446x_prepare(const void *payload, unsigned short payload_len)
{
//	uint8_t total_len;
//	int i;
 
//	PRINTF("si446x: sending %d bytes\n\r", payload_len);

//	RIMESTATS_ADD(lltx);

	 // PRINTF("payload Contents :\n\r");
	 // for(i=0; i<payload_len; i++)
//	  {
	//	  PRINTF("%02X ",((uint8_t *)payload)[i]);
	//	  if((i%16) == 15) PRINTF("\n\r");
	//  }
	//  PRINTF("\n\r");

//	total_len = payload_len;
 // si446x_write_tx_fifo(payload_len, payload);
  
	return 0;
}

/*---------------------------------------------------------------------------*/
static int
si446x_transmit(unsigned short transmit_len)
{
  // Transmit on channel 0 only, immediatly, return to ready
 si446x_start_tx(0, 0x30, transmit_len  );
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int
si446x_send(const void *payload, unsigned short payload_len)
{
 si446x_prepare(payload, payload_len);
 return si446x_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int
si446x_read(void *buf, unsigned short buf_len)
{
  //uint8_t footer[FOOTER_LEN];
  //uint8_t len;
  
  //si446x_packets_read++;
  //si446x_read_rx_fifo(1, &len);
  

  //if(len > SI466X_FIFO_SIZE) {
	/* Oops, we must be out of sync. */
//	flushrx();
	//return 0;
 // }

  //if(len <= AUX_LEN) {
	//flushrx();
//	return 0;
 // }

  //if(len - AUX_LEN > buf_len) {
	//flushrx();
//	return 0;
 // }


  /* Check RX Fifo Error */
 /* if( status() == CC112X_STATE_RXFIFO_ERROR )
  {
	  flushrx();
	  return 0;
  }*/

  //si446x_read_rx_fifo(len - AUX_LEN, buf);
  //si446x_read_rx_fifo(FOOTER_LEN, &footer);

//#ifdef KLKTEST_RX
//#if 1
  //int i=0;
  /* Dump Packet */
 // PRINTF("Packet received (%d bytes) :\n\r",len);
  //for(i=0; i<len; i++)
  //{
	  //PRINTF("%02X ",((uint8_t *)(buf))[i]);
	  //if((i%16) == 15) PRINTF("\n\r");
 // }
  //PRINTF("\n\r");
  //PRINTF("RSSI = %d\n\r", footer[0] );
//#endif

  //if(footer[1] & FOOTER1_CRC_OK) {
	//si446x_last_rssi = footer[0];
	//si446x_last_lqi = footer[1] & FOOTER1_CORRELATION;


	//packetbuf_set_attr(PACKETBUF_ATTR_RSSI, si446x_last_rssi);
	//packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, si446x_last_lqi);

  //} else {
	//PRINTF("Bad CRC");
	//len = AUX_LEN;
  //}

  //if(len < AUX_LEN) {
	//return 0;
  //}

  //return len - AUX_LEN;
}

/*---------------------------------------------------------------------------*/
static int
si446x_channel_clear(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
si446x_receiving_packet(void)
{
  return 1;//gRadio_CheckReceived();
}
/*---------------------------------------------------------------------------*/
static int
si446x_pending_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
si446x_on(void)
{
  //si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
si446x_off(void)
{
  //si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
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


