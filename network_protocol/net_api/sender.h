#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "utils/packet.h"
#include "utils/net_list.h"



/*
    Encode and send packet using the mode you want.
    UNICAST: packet is relayed through the network to the destination
    BROADCAST: packet is read and relayed by every node in the network (The gateway will not reflect sub network between them)
    MULTICAST: packet is send from server to a group of node (irrigation or light_bulb)
*/
void send(packet_t* packet, int mode);


//void unreliable_send(packet_t* packet, int mode) { send(packet, mode);}     // TO not change the old API

void send_dst(packet_t* packet, int mode, linkaddr_t* dst);
