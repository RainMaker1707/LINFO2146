#include "broadcast.h"



bool contains_rly(packet_t* packet){
    uint8_t* flags = retrieve_flags(packet);
    // flags == {TCP, MLT, ACk, NACK, RLY, DIO, DIS, PRT}
    return flags[4] == 1;
}


void broadcast_redirection(packet_t* packet){
    if(get_rank() == GATEWAY) return; // Gateway don't broadcast information comming from subgateway in subnetwork
    if(!contains_rly(packet)) packet->flags += RLY;
    if(get_parent() != NULL && !linkaddr_cmp(packet->src, (linkaddr_t*)&get_parent()->adress)) { // If message come from parent
        LOG_INFO("RETRANSMIT TO PARENT %d\n", packet->flags);
        //send to parent
        send_dst(packet, UNICAST, (linkaddr_t*)&get_parent()->adress);
    }

    LOG_INFO("RETRANSMIT TO CHILD %d\n", packet->flags);    
    if(get_childs()->head == NULL) return;
    // send to childs
    node_t* node = get_childs()->head;
    
    while(1){
        if(linkaddr_cmp((linkaddr_t*)&node->mote->adress, packet->src)) {
            send_dst(packet, UNICAST, (linkaddr_t*)&node->mote->adress);
        }
        if(node == get_childs()->tail) return;
        node = node->next;
    }
}