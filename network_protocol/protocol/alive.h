#pragma once

#include "constants/flags.h"
#include "constants/types.h"
#include "constants/constant.h"


#include "utils/packet.h"

#include "net_api/sender.h"
#include "net_api/setup.h"

#include "protocol/parent.h"

#include "sys/clock.h"
#include "sys/log.h"


#define MAX_DELAY_ALIVE 60

void heartbeat(int rank);

void alive(packet_t* packet);

void check_neighbors(list_t* list);

node_t* find_child(list_t* list, linkaddr_t* addr);