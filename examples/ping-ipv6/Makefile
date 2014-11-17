all: example-ping6
APPS=ping6

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS+=-DUIP_CONF_IPV6 -DWITH_UIP6

SMALL=1

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
