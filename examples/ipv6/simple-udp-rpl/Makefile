CONTIKI_PROJECT += unicast-sender unicast-receiver broadcast-example
APPS=servreg-hack
CONTIKI=../../..

all: $(CONTIKI_PROJECT) $(CONTIKI_PROJECT).size $(CONTIKI_PROJECT).bin $(CONTIKI_PROJECT).upload
	@echo " "
	@echo "** Build done **************************************"
	@echo " "

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS+= -DUIP_CONF_IPV6_RPL

include $(CONTIKI)/Makefile.include
