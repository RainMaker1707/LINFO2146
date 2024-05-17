#include "utils/packet.h"
#include "utils/net_list.h"

#include "constants/constant.h"
#include "constants/flags.h"

#include "net_api/sender.h"
#include "net_api/setup.h"

#include "net/packetbuf.h"



void send_discover();
void discover(packet_t* packet, const linkaddr_t *src, const linkaddr_t *dest);
