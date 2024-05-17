CONTIKI_PROJECT = node
CONTIKI = ../..

PROJECT_SOURCEFILES += utils/packet.c utils/net_list.c 
PROJECT_SOURCEFILES += net_api/receiver.c net_api/sender.c net_api/setup.c 
PROJECT_SOURCEFILES += protocol/alive.c protocol/discover.c protocol/parent.c protocol/unicast.c protocol/broadcast.c

all: $(CONTIKI_PROJECT)

$(shell mkdir -p build/z1/obj/utils)
$(shell mkdir -p build/z1/obj/protocol)
$(shell mkdir -p build/z1/obj/net_api)

MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include