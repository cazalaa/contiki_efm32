CONTIKI_PROJECT = udp-client udp-server udp-client-temperature
APPS=servreg-hack
CONTIKI=../../..

WITH_UIP6=1
UIP_CONF_IPV6=1

CFLAGS+= -DUIP_CONF_IPV6_RPL

ifdef WITH_COMPOWER
APPS+=powertrace
CFLAGS+= -DCONTIKIMAC_CONF_COMPOWER=1 -DWITH_COMPOWER=1 -DQUEUEBUF_CONF_NUM=4
endif

ifdef SERVER_REPLY
CFLAGS+=-DSERVER_REPLY=$(SERVER_REPLY)
endif
ifdef PERIOD
CFLAGS+=-DPERIOD=$(PERIOD)
endif

all: $(CONTIKI_PROJECT) $(CONTIKI_PROJECT).size $(CONTIKI_PROJECT).bin $(CONTIKI_PROJECT).upload
	@echo " "
	@echo "** Build done **************************************"
	@echo " "

include $(CONTIKI)/Makefile.include
