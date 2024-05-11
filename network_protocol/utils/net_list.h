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


/*
    Simple O(n) search
*/
bool list_contains_src(list_t* list, linkaddr_t* src){
    if(list->head != NULL){
        list->current = list->head;
        while(1){
            if(linkaddr_cmp(list->current->mote->adress, src)) return true;
            if(list->current == list->tail) break;
            list->current = list->current->next;
        }
    }
    return false;
}
