#include <stdlib.h>
#include <stdbool.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"


#include "../utils/packet.h"
#include "../utils/net_list.h"

#include "protocol/alive.h"
#include "protocol/discover.h"
#include "protocol/unicast.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2

#define PACKET_MAX_LEN PACKET_SIZE           // packet are 32 bytes max

#define ALIVE_MAX_INTERVAL 15

#define GATEWAY 2
#define SUBGATEWAY 1
#define SENSOR 0
