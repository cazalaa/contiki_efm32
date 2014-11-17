CONTIKI_PROJECT = udp-server udp-client
all: $(CONTIKI_PROJECT)

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS+= -DWITH_UIP6 -DUIP_CONF_IPV6 


CONTIKI = ../..
include $(CONTIKI)/Makefile.include
