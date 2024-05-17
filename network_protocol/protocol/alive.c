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
    if(neigh == NULL) {
        printf("No neighbors found\n");
    }else neigh->mote->last_time_heard = clock_time();
}



void check_neighbors(list_t* list){
    if(list->head == NULL) return;
    unsigned long long interval = MAX_DELAY_ALIVE * CLOCK_SECOND;
    node_t* current = list->head;
    node_t* tmp = current;
    while(1){
        if(clock_time() - current->mote->last_time_heard > interval){
            printf("Need to delete neighbor\n");
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


