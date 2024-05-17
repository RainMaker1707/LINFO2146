#include "parent.h"



/*
    Send a packet with PRT flag to ask a node to be parent
*/
void send_prt(linkaddr_t* dest){
    printf("send PRT\n");
    if(list_contains_src(get_childs(), dest) || linkaddr_cmp((linkaddr_t*)&get_parent()->adress, dest)) return;
    packet_t* packet = create_packet(get_type(), PRT, get_rank(), &linkaddr_node_addr, dest, "PARENT");
    send(packet, UNICAST);
    free_packet(packet);
}

void send_prt_ack(linkaddr_t* dest){
    printf("PRT ACK\n");
    packet_t* packet = create_packet(get_type(), PRT+ACK, get_rank(), &linkaddr_node_addr, dest, "PARENT");
    send(packet, UNICAST);
    free_packet(packet);
}

void send_prt_nack(linkaddr_t* dest){
    printf("PRT NACK\n");
    packet_t* packet = create_packet(get_type(), PRT+NACK, get_rank(), &linkaddr_node_addr, dest, "PARENT");
    send(packet, UNICAST);
    free_packet(packet);
}



/*
    Function that define the behavior when your receive a PRT packet
*/
void receive_prt(packet_t* packet, linkaddr_t* src){
    if(get_parent_config() && get_parent()==NULL) send_prt_nack(packet->src);
    else if(linkaddr_cmp((linkaddr_t*)&get_parent()->adress, packet->src)) {
        LOG_INFO("PARENT ADDR\n");
        send_prt_nack(packet->src);
    }else{
        add_child(get_childs(), find_child(get_neighbors(), packet->src)->mote);
        send_prt_ack(packet->src);
    }
}



/*
*/
void receive_prt_nack(packet_t* packet){
    if(list_contains_src(get_childs(), packet->src)) {
        remove_child(get_childs(), packet->src);
    }
}



/*
    Attach parent IFF no parent is setup OR better signal strength with same rank is found
*/
void attach_parent(packet_t* packet, linkaddr_t* addr){
    // TODO: check signal strength
    if(list_contains_src(get_childs(), packet->src)) return;
    if((get_parent_config() && get_parent() == NULL)        // If need parent but not have one
        || get_rank()+1 == packet->src_rank                 // If rank is equal to the superior rank // TODO: check here because of behavior changement on +1 or not
        || compare_signal_strength(get_parent(), find_child(get_neighbors(), packet->src)->mote) < 0)
    {
        if(get_parent() != NULL) {
            LOG_INFO("HERE\n");
            send_prt_nack((linkaddr_t*)&get_parent()->adress); // say to node he is no more our parent
            set_parent(NULL); // reset parent before change
        }
        if(find_child(get_neighbors(), packet->src) == NULL) LOG_INFO("PARENT NOT FIND IN NEIGHBOR\n");
        else if(linkaddr_cmp((const linkaddr_t*)packet->src, &linkaddr_null)) LOG_INFO("Cannot attach NULL as parent\n");
        else set_parent(find_child(get_neighbors(), packet->src)->mote);
    }
}



/*
    return X an int to indicate which mote has the higher signal strength
        if X == 0: signal are the same
        if X > 0: mote1 has the highest signal
        if X < 0: mote2 has the highest signal
*/
int compare_signal_strength(mote_t* mote1, mote_t* mote2){
    return mote1->signal_strenght - mote2->signal_strenght;
}


/*
*/
void remove_child(list_t* list, linkaddr_t*  addr){
    if(list->head == NULL || addr == NULL) return;
    node_t* current = list->head;
    node_t* tmp = list->head;
    while(1){
        if(linkaddr_cmp(addr, (linkaddr_t*)&current->mote->adress)){
            if(get_parent() == current->mote) set_parent(NULL);
            if(list->head==list->tail){
                free_node(current);
                list->head = NULL;
                list->tail = NULL;
                current = NULL;
                return;
            }
            if(current == list->head){
                list->head = current->next;
                free_node(current);
                current = list->head;
                list->tail->next = list->head;
            } else if(current == list->tail){
                tmp->next = list->head;
                free_node(current);
                list->tail = tmp;
            }else{
                tmp->next = current->next;
                free_node(current);
                current = tmp;
            }
            return;
        }
        if(current == list->tail) return;
        tmp = current;
        current = current->next;
    }
}