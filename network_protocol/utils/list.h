#include <stdlib.h>
#include <stdio.h>

typedef struct mote{
    int rank;
    char* adress;
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