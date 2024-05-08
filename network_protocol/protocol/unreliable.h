#include "../util.h"

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2



typedef struct node{
    int rank;
    char* adress;
    int signal_strenght;
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



/*
    Encode and send the packet to the address (mode=UNICAST), group address (mode=MULTICAST)
     or all address (mode=BROADCAST)
    Unreliable mean you will maybe not have an answer, and no packet will be acknowledged
    @Param: packet_t* packet: the well formated packet to encode and send
    @Param: int mode: the mode constant representing the sending mode
    @Param: char* adress: the adress or group adress used to send the packet (can be NULL for broadcast only)
*/
void unreliable_send(packet_t* packet, int mode, char* address);

/*
*/
neighbor_list_t* discover_neighbor();

/*
*/
void attach_parent(neighbor_list_t* neighbor_list);

/*
*/
child_list_t* create_child_list();

/*
*/
void add_child(child_list_t* child_list);

/*
*/
void free_child_list(child_list_t* child_list);

/*
*/
void unreliable_wait_receive(char * buffer);
