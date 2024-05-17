#include "alive.h"





void heartbeat(int rank){
    packet_t* packet = create_packet(TYPE_BULB, DIO, rank, &linkaddr_node_addr, NULL, "ALIVE");
    send(packet, BROADCAST);
    free_packet(packet);
}



node_t* find_child(list_t* list, linkaddr_t* addr){
    if(list->head == NULL) return NULL;
    node_t* current = list->head;
    while(1){
        printf("\n");
        if(linkaddr_cmp((linkaddr_t*)&(current->mote->adress), addr)) return current;
        if(current == list->tail) return NULL;
        current = current->next;
    }
}



void alive(packet_t* packet){
    node_t* neigh = find_child(get_neighbors(), packet->src);
    if(neigh != NULL){
        // UPDATE MOTE LAST TIME HEARD TO NOW
        neigh->mote->last_time_heard = clock_time();
        // UPDATE MOTE SIGNAL STRENGTH
        int16_t rssi = (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
        neigh->mote->signal_strenght = rssi;
        // SEND PRT to new parent concurrent
        printf("TEST: %d, RANK %d -- SRCRK %d\n",packet->src_rank == get_parent()->rank+1, get_parent()->rank, packet->src_rank); 
        if(get_parent_config()){
            if(get_parent() == NULL
            || (get_parent()->signal_strenght < rssi && get_parent()->rank == packet->src_rank) 
            || packet->src_rank == get_parent()->rank+1
            )
        {
            printf("SEND PRT\n");
            send_prt(packet->src);
        }
        }
    }
}



void check_neighbors(list_t* list){
    if(list->head == NULL) return;
    unsigned long long interval = MAX_DELAY_ALIVE * CLOCK_SECOND;
    node_t* current = list->head;
    node_t* tmp = current;
    while(1){
        if(clock_time() - current->mote->last_time_heard > interval){
            if(get_parent() == current->mote) set_parent(NULL);
            if(list->head==list->tail){
                free(current->mote);
                free(current);
                list->head = NULL;
                list->tail = NULL;
                current = NULL;
                return;
            }
            if(current == list->head){
                list->head = current->next;
                free(current->mote);
                free(current);
                current = list->head;
                list->tail->next = list->head;
            } else if(current == list->tail){
                tmp->next = list->head;
                free(current->mote);
                free(current);
                list->tail = tmp;
            }else{
                tmp->next = current->next;
                free(current->mote);
                free(current);
                current = tmp;
            }

        }
        if(current == list->tail) return;
        tmp = current;
        current = current->next;
    }
}


