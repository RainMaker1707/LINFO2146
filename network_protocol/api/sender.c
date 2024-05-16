#include "sender.h"

void unreliable_send(const linkaddr_t* dest, uint8_t* buffer){
    //nullnet_len = PACKET_SIZE;
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    NETSTACK_NETWORK.output(dest);
    
}


void send(packet_t* packet, int mode){
    if(mode == MULTICAST) packet->flags += MLT;
    uint8_t* buffer = (uint8_t*)encode(packet);
    switch(mode){
        case UNICAST:
            unreliable_send(packet->dst, buffer);
            break;
        case BROADCAST:
            unreliable_send(&linkaddr_null, buffer);
            break;
        case MULTICAST:
            packet->flags += MLT;
            unreliable_send(packet->dst, buffer);
            break;
        default:
            printf("Mode not allowed, only BROADCAST, UNICAST or MULTICAST allowed.\n");
    }
    free(buffer);
}