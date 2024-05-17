#include "parent.h"



/*
    Send a packet with PRT flag to ask a node to be parent
*/
void send_prt(linkaddr_t* dest){
    LOG_INFO("PRT SENT\n");
    if(list_contains_src(get_childs(), dest) || linkaddr_cmp((linkaddr_t*)&get_parent()->adress, dest)) return;
    if(get_rank() == GATEWAY) return;
    packet_t* packet = create_packet(get_type(), PRT, get_rank(), &linkaddr_node_addr, dest, "PARENT");
    send(packet, UNICAST);
    free_packet(packet);
}

void send_prt_ack(linkaddr_t* dest){
    LOG_INFO("PRT ACK SENT\n");
    if(get_parent() != NULL && linkaddr_cmp((linkaddr_t*)&get_parent()->adress, dest)) send_prt_nack(dest);
    else{
        packet_t* packet = create_packet(get_type(), PRT+ACK, get_rank(), &linkaddr_node_addr, dest, "PARENT");
        send(packet, UNICAST);
        free_packet(packet);
    }
}

void send_prt_nack(linkaddr_t* dest){
    LOG_INFO("PRT NACK SENT ");
    LOG_INFO_LLADDR(dest);
    printf("\n");
    packet_t* packet = create_packet(get_type(), PRT+NACK, get_rank(), &linkaddr_node_addr, dest, "PARENT");
    send(packet, UNICAST);
    free_packet(packet);
}



/*
    Function that define the behavior when your receive a PRT packet
*/
void receive_prt(packet_t* packet, linkaddr_t* src){
    if(get_parent_config() && get_parent()==NULL) {
        LOG_INFO("IN BUG 0\n");
        send_prt_nack(packet->src);
    }else if(get_rank() == packet->src_rank+2) {
        LOG_INFO("IN BUG 1\n");
        send_prt_nack(packet->src);
    }else if(linkaddr_cmp((linkaddr_t*)&get_parent()->adress, packet->src)) {
        LOG_INFO("IN BUG 2\n");
        send_prt_nack(packet->src);
    }else{
        node_t* node = find_child(get_neighbors(), packet->src);
        if(node == NULL) {
            LOG_INFO("CHILDS empty\n");
            return;
        }
        LOG_INFO("HERE\n");
        add_child(get_childs(), node->mote);
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
    if(get_rank() > packet->src_rank) return;
    if(list_contains_src(get_childs(), packet->src)) return;
    if(get_parent() != NULL && get_parent()->rank > packet->src_rank) return;
    if((get_parent_config() && get_parent() == NULL)        // If need parent but not have one
        || (get_rank()+1 == packet->src_rank && get_parent()->rank == get_rank()) // If rank is equal to the superior rank
        || (get_parent()->signal_strenght < (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI) && (get_parent()->rank == packet->src_rank)) // If node has same rank but better signal
        ) 
    {
        LOG_INFO("ENTERED\n");
        if(get_parent() != NULL) {
            LOG_INFO("RESET PARENT\n");
            send_prt_nack((linkaddr_t*)&get_parent()->adress); // say to node he is no more our parent
            set_parent(NULL); // reset parent before change
        }
        if(find_child(get_neighbors(), packet->src) == NULL) LOG_INFO("PARENT NOT FIND IN NEIGHBOR\n");
        else if(linkaddr_cmp((const linkaddr_t*)packet->src, &linkaddr_null)) LOG_INFO("Cannot attach NULL as parent\n");
        else set_parent(find_child(get_neighbors(), packet->src)->mote);
    }
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