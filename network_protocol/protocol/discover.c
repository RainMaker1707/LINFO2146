#include "discover.h"



void send_discover(){
    packet_t* packet = create_packet(get_type(), DIS, get_rank(), (const linkaddr_t*)&linkaddr_node_addr, (const linkaddr_t*)&linkaddr_null, "DISCOVER");
    send(packet, BROADCAST);
    free_packet(packet);
}

void send_discover_ack(linkaddr_t* dst){
    packet_t* packet_sd = create_packet(get_type(), DIS+ACK, get_rank(), (const linkaddr_t*)&linkaddr_node_addr, dst, "DISC+ACK");
    send(packet_sd, UNICAST);
    free_packet(packet_sd);
}


void discover(packet_t* packet, const linkaddr_t *src, const linkaddr_t *dest){
    if(!list_contains_src(get_neighbors(), packet->src)){
        // ADD NEIGHBORS
        mote_t* mote = create_mote(packet->type, packet->src_rank, packet->src, (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI), src);
        add_child(get_neighbors(), mote);
    }
    // RESPOND DIS+ACK for all DIS received
    if(packet->flags == DIS+ACK) return;
    send_discover_ack((linkaddr_t*)src);
}