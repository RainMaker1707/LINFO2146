#include "receiver.h"



list_t* childs = NULL;
mote_t* parent = NULL;
list_t* neighbors = NULL;

bool accept_childs_config = false; // By default mote doesn't accept child
bool need_parent_config = true; // By default mote accept parent;
uint8_t node_rank = 2; // By default mote is a sensor (node)


typedef void (*fct_ptr)(packet_t* packet);
fct_ptr node_callback;


void switch_response(packet_t* packet){
    switch(packet->flags){
        case UDP:
            node_callback(packet);
            break;
        case DIS:
            break;
        case DIO:
            break;
        case DIS+ACK:
            break;
        case PRT:
            break;
        case PRT+ACK:
            break;
        case PRT+NACK:
            break;
    }
}

/*
*/
void receive(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest){
    printf("PACKET RECEIVED\n");
    packet_t* packet = decode((char*)data);
    //if(packet == NULL) return;
    //if(!linkaddr_cmp(packet->dst, &linkaddr_node_addr) && !linkaddr_cmp(packet->dst, &linkaddr_null)) return;
    switch_response(packet);
    free_packet(packet);
}

void setup(uint8_t rank, bool accept_child, bool need_parent, void* callback){
    
    
}


// ############################### SETUP API ###############################


static uint8_t buffer[PACKET_SIZE];


/*
    Create neighbors list
    Choose a parent with appropriate rank and maximum signal strength, then initialise parent
    Create the child_list (empty at first by can add child if chosen as parent)
    @Param: accept_childs: if true then the mote will accept child when receive a PRT packet
                            by default set as false
*/
void setup_gateway(uint8_t rank, bool accept_child, bool need_parent, void* callback){
    // NULLNET config
    nullnet_buf = buffer;
    nullnet_len = PACKET_SIZE;
    
    nullnet_set_input_callback(receive);
    // SETUP inner list
    if(accept_child) childs = create_list(); // empty at first, only add when asked to be parent and accept
    neighbors = create_list();
    accept_childs_config = accept_child; // To know if node accept childs
    need_parent_config = need_parent;    // To know if node accept parent
    node_rank = rank;
    node_callback = (fct_ptr)callback;
    LOG_INFO("Node setup ok\n");
}

void setup_subgateway(uint8_t rank, bool accept_child, void* callback){
    setup_gateway(rank, accept_child, true, callback);
}


void setup_node(uint8_t rank, void* callback) {
    setup_subgateway(rank, false, callback);
}