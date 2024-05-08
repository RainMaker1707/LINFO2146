#include "../util.h"

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2



typedef struct node{
    int rank;
    char* adress;
    unsigned long long last_time_heard;
} node_t;

typedef node_t child_t;
typedef node_t parent_t;

typedef struct child_list{
    child_t* head;
    child_t* tail;
    child_t* current;
}child_list_t;


typedef child_list_t neighbor_list_t;




void unicast_unreliable_send(char* buffer, int mode, char* adress);
void broadcast_unreliable_send(char* buffer, int mode);
void multicast_unreliable_send(char* buffer, int mode, char* group_address);

neighbor_list_t* discover_neighbor();
void search_parent(neighbor_list_t* neighbor_list);

child_list_t* create_child_list();
void search_child(child_list_t* child_list);

void unreliable_wait_receive(char * buffer);
