#include "unicast.h"


void unicast_redirection(packet_t* packet){
    if(list_contains_src(get_childs(), packet->dst)) {
        LOG_INFO("SEND TO CHILD\n");
        send_dst(packet, UNICAST, (linkaddr_t*)&find_child(get_neighbors(), packet->dst)->mote->src);
    }else {
        LOG_INFO("SEND TO PARENT\n");
        send_dst(packet, UNICAST, (linkaddr_t*)&get_parent()->adress);
    }
}