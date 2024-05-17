#include "broadcast.h"



bool contains_rly(packet_t* packet){
    uint8_t* flags = retrieve_flags(packet);
    // flags == {TCP, MLT, ACk, NACK, RLY, DIO, DIS, PRT}
    return flags[4] == 1;
}


void broadcast_redirection(packet_t* packet, linkaddr_t* src){
    if(get_parent() == NULL && get_childs()->head == NULL) return;
    if(get_rank() == GATEWAY) return; // Gateway don't broadcast information comming from subgateway in subnetwork
    // If message doesn't come from parent or child discard it
    if(get_parent() != NULL && !linkaddr_cmp(packet->src, (linkaddr_t*)&get_parent()->adress) && !list_contains_src(get_childs(), src)) {
        return;
    }
    if(!contains_rly(packet)) packet->flags += RLY;
    if(get_parent() != NULL && !linkaddr_cmp(packet->src, (linkaddr_t*)&get_parent()->adress)) { // If message doesn't come from parent
        //send to parent
        send_dst(packet, UNICAST, (linkaddr_t*)&get_parent()->adress);
    }   
    if(get_childs()->head == NULL) return;
    // send to childs
    node_t* node = get_childs()->head;
    while(1){
        if(!linkaddr_cmp((linkaddr_t*)&node->mote->adress, packet->src)) {
            send_dst(packet, UNICAST, (linkaddr_t*)&node->mote->adress);
        }
        if(node == get_childs()->tail) return;
        node = node->next;
    }
}