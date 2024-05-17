#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "sender.h"

#include "protocol/alive.h"
#include "protocol/discover.h"
#include "protocol/parent.h"
#include "protocol/unicast.h"
#include "protocol/broadcast.h"

#include "setup.h"

#include "utils/packet.h"
#include "utils/net_list.h"



#define PACKET_MAX_LEN PACKET_SIZE           // packet are 32 bytes max


/*
*/
void switch_response(packet_t* packet, const linkaddr_t *src, const linkaddr_t *dest);


/*
*/
void receive(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest);
