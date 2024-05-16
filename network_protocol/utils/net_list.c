#include "net_list.h"


// ########################  API  #############################


list_t* create_list(){
    list_t* list = malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    return list;
}


void add_child(list_t* list, mote_t* mote){
    if(mote == NULL ||list == NULL) return;
    node_t* node = malloc(sizeof(node_t));
    node->mote = mote;
    if(list->head == NULL){
        node->next = node;
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        node->next = list->head;
        list->tail = node;
    }
}


void free_list(list_t* list){
    node_t* to_free = list->head;
    node_t* current = list->head;
    while(current != list->tail){
        to_free = current;
        current = current->next;
        free(to_free->mote);
        free(to_free);
    }
    free(current);
    free(list);
}

mote_t* create_mote(int rank, const linkaddr_t* adress, int signal_strenght, const linkaddr_t* src){
    mote_t* mote = malloc(sizeof(mote_t));
    mote->rank = rank;
    linkaddr_copy((linkaddr_t*)&(mote->adress), adress);
    mote->signal_strenght = signal_strenght;
    mote->last_time_heard = clock_time();
    linkaddr_copy((linkaddr_t*)&(mote->src), src);
    return mote;
}


/*
    Simple O(n) search
*/
bool list_contains_src(list_t* list, linkaddr_t* src){
    if(list->head != NULL){
        node_t* current = list->head;
        while(1){
            if(linkaddr_cmp((const linkaddr_t*)&(current->mote->adress), src)) return true;
            if(current == list->tail) break;
            current = current->next;
        }
    }
    return false;
}
