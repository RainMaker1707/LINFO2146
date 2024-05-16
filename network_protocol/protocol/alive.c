#include "alive.h"


void heartbeat(int rank){
    packet_t* packet = create_packet(TYPE_BULB, DIO, rank, &linkaddr_node_addr, NULL, "ALIVE");
    send(packet, BROADCAST);
    free_packet(packet);
}