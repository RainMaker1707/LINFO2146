#include "list.h"


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



// ONLY for test purposes

// int main(){
//     list_t* list = create_list();
//     mote_t* mote = malloc(sizeof(mote_t));
//     mote->adress = "OK";
//     mote_t* mote2 = malloc(sizeof(mote_t));
//     mote2->adress = "OK2";
//     add_child(list, mote);
//     add_child(list, mote2);
//     list->current = list->head;
//     while(list->current != list->tail){
//         printf("ADRESS: %s\n", list->current->mote->adress);
//         list->current = list->current->next;
//     }
//     printf("ADRESS: %s\n", list->tail->mote->adress);
//     exit(0);
// }