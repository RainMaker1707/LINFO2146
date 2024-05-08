#include "../utils/packet.h"
#include "../utils/list.h"

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2




/*
    Encode and send the packet to the address (mode=UNICAST), group address (mode=MULTICAST)
     or all address (mode=BROADCAST)
    Unreliable mean you will maybe not have an answer, and no packet will be acknowledged
    @Param: packet_t* packet: the well formated packet to encode and send
    @Param: int mode: the mode constant representing the sending mode
    @Param: char* adress: the adress or group adress used to send the packet (can be NULL for broadcast only)
*/
void unreliable_send(packet_t* packet, int mode, char* address);

/*
*/
list_t* discover_neighbor();

/*
*/
void attach_parent(mote_t* mote);

/*
*/
void unreliable_wait_receive(char * buffer);
