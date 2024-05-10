#include <stdlib.h>
#include <stdio.h>
#include "sys/clock.h"
#include "net/netstack.h"

typedef struct mote{
    int rank;
    const linkaddr_t* adress;
    int signal_strenght;
    unsigned long long last_time_heard;
} mote_t;

typedef struct node {
    mote_t* mote;
    struct node* next;
} node_t;

typedef struct list{
    node_t* head;
    node_t* tail;
    node_t* current;
}list_t;

/*
*/
list_t* create_list();

/*
*/
void add_child(list_t* list, mote_t* mote);

/*
*/
void free_list(list_t* list);

/*
*/
mote_t* create_mote(int rank, const linkaddr_t* adress, int signal_strenght);


list_t* create_list(){
    list_t* list = malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    return list;
}


void add_child(list_t* list, mote_t* mote){
    if(mote == NULL) {
        printf("\033[33mError in add_child(): param node can't be NULL\033[0m\n");
        exit(1);
    }
    if(list == NULL) {
        printf("\033[33mError in add_child(): param list can't be NULL\033[0m\n");
        exit(1);
    }
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
    list->current = list->head;
    while(list->current != list->tail){
        to_free = list->current;
        list->current = list->current->next;
        free(to_free->mote);
        free(to_free);
    }
    free(list->current);
    free(list);
}

mote_t* create_mote(int rank, const linkaddr_t* adress, int signal_strenght){
    mote_t* mote = malloc(sizeof(mote_t));
    mote->rank = rank;
    mote->adress = adress;
    mote->signal_strenght = signal_strenght;
    mote->last_time_heard = clock_time();
    return mote;
}


node_t* find_neighbor_in_list(list_t* list, const linkaddr_t* address){
    node_t* current = list->head;
    while(current != list->tail){
        if(linkaddr_cmp(current->mote->adress, address)){
            return current;
        }
        current = current->next;
    }

    if(linkaddr_cmp(current->mote->adress, address)){
        return current;
    }
    return NULL;
}

void set_mote_last_time(list_t* list, node_t* node){
    list->current = list->head;
    while(list->current != list->tail){
        if(list->current == node){
            list->current->mote->last_time_heard = clock_time();
            return;
        }
        list->current = list->current->next;
    }
    if(list->current == node){
        list->current->mote->last_time_heard = clock_time();
        return;
    }
}


/*
  set the last time heard parameter of the neighbor to the current cpu time
*/
void neighbor_is_alive(list_t* list, const linkaddr_t* neigh_address){
    if(list == NULL){
        printf("\033[33mError in neighbor_is_alive(): param list can't be NULL\033[0m\n");
        exit(1);
    }
    if(neigh_address == NULL){
        printf("\033[33mError in neighbor_is_alive(): param neigh_address can't be NULL\033[0m\n");
        exit(1);
    }
    node_t* neighbor = find_neighbor_in_list(list, neigh_address);
    if(neighbor == NULL){
        //TODO maybe do something if not in the list like send a DIS ?
        printf("Neighbor is not in the list\n");
        return;
    }
    printf("setting time for neighbor\n");
    set_mote_last_time(list, neighbor);
}



/*
 function that loops through the list of neighbors and check if the last time heard is too old

*/
void process_neighbors_last_time(list_t* list){
    if(list == NULL){
        printf("\033[33mError in process_neighbors_last_time(): param list can't be NULL\033[0m\n");
        exit(1);
    }
    long alive_difference = 20 * CLOCK_SECOND;

    list->current = list->head;
    if(list->head == list->tail){
        printf("time difference is %ld\n", (long) (clock_time() - list->current->mote->last_time_heard));
        printf("alive difference is %ld\n", alive_difference);
        if(clock_time() - list->current->mote->last_time_heard > alive_difference){
            free(list->current->mote);
            free(list->current);
            list->head = NULL;
            list->tail = NULL;
            list->current = NULL;
            printf("removed neighbor from list\n");
            return;
        }
    }
    
    while(list->current != list->tail){
        printf("Checking neighbor while loop\n");
        if(clock_time() - list->current->mote->last_time_heard > alive_difference){
            free(list->current->mote);
            free(list->current);
            list->head = NULL;
            list->tail = NULL;
            list->current = NULL;
            return;
        }
        list->current = list->current->next;
    }
    printf("Checking neighbor is done\n");

}

