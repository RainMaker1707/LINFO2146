#include "import.h"


/*
    Encode and send packet using the mode you want.
    UNICAST: packet is relayed through the network to the destination
    BROADCAST: packet is read and relayed by every node in the network (The gateway will not reflect sub network between them)
    MULTICAST: packet is send from server to a group of node (irrigation or light_bulb)
*/
void send(packet_t* packet, int mode);
