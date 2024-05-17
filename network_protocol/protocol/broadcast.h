#include "utils/packet.h"
#include "utils/net_list.h"

#include "net_api/setup.h"
#include "net_api/sender.h"


bool contains_rly(packet_t* packet);

void broadcast_redirection(packet_t* packet);