CONTIKI_PROJECT = node
CONTIKI = ../..

PROJECT_SOURCEFILES += utils/packet.c utils/net_list.c api/receiver.c api/sender.c protocol/alive.c protocol/discover.c protocol/unicast.c

all: $(CONTIKI_PROJECT)

$(shell mkdir -p build/z1/obj/utils)
$(shell mkdir -p build/z1/obj/protocol)
$(shell mkdir -p build/z1/obj/api)

MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include