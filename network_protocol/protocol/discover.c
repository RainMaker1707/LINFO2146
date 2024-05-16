#include "discover.h"



void send_discover(){
    packet_t* packet = create_packet(get_type(), DIS, get_rank(), (const linkaddr_t*)&linkaddr_node_addr, (const linkaddr_t*)&linkaddr_null, "DISCOVER");
    send(packet, BROADCAST);
    free_packet(packet);
}


void discover(packet_t* packet, const linkaddr_t *src, const linkaddr_t *dest){
    if(!list_contains_src(get_neighbors(), packet->src)){
        // ADD NEIGHBORS
        mote_t* mote = create_mote(packet->type, packet->src_rank, packet->src, 10, src);
        add_child(get_neighbors(), mote);
        LOG_INFO("ADD NEIGHBORS\n");
    }
    if(packet->flags == DIS+ACK) return;
    packet_t* packet_sd = create_packet(get_type(), DIS+ACK, get_rank(), (const linkaddr_t*)&linkaddr_node_addr, src, "DISC+ACK");
    send(packet_sd, UNICAST);
    free_packet(packet_sd);
}