#include <stdlib.h>
#include <stdio.h>
#include "sys/clock.h"
#include "net/netstack.h"

typedef struct mote{
    int rank;
    linkaddr_t adress;
    int signal_strenght;
    unsigned long long last_time_heard;
    linkaddr_t src;
} mote_t;

typedef struct node {
    mote_t* mote;
    struct node* next;
} node_t;

typedef struct list{
    node_t* head;
    node_t* tail;
}list_t;



list_t* create_list();
void add_child(list_t* list, mote_t* mote);
void free_list(list_t* list);
mote_t* create_mote(int rank, const linkaddr_t* adress, int signal_strenght, const linkaddr_t* src);
bool list_contains_src(list_t* list, linkaddr_t* src);