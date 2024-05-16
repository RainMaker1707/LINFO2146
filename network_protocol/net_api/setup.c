#include "setup.h"


list_t* childs = NULL;
mote_t* parent = NULL;
list_t* neighbors = NULL;

bool accept_childs_config = false; // By default mote doesn't accept child
bool need_parent_config = true; // By default mote accept parent;
uint8_t node_rank = 2; // By default mote is a sensor (node)
uint8_t node_type = 0; // ERROR type by default

fct_ptr node_callback;


fct_ptr get_callback(){
    return node_callback;
}

uint8_t get_rank(){
    return node_rank;
}

uint8_t get_type(){
    return node_type;
}


list_t* get_neighbors() { 
    return neighbors;
}

mote_t* get_parent(){
    return parent;
}

void set_parent(mote_t* mote){
    parent = mote;
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
void setup_gateway(uint8_t type, uint8_t rank, bool accept_child, bool need_parent, void* callback){
    // NULLNET config
    nullnet_buf = buffer;
    nullnet_len = PACKET_SIZE;
    
    nullnet_set_input_callback(receive);
    node_type = type;
    // SETUP inner list
    if(accept_child) childs = create_list(); // empty at first, only add when asked to be parent and accept
    neighbors = create_list();
    accept_childs_config = accept_child; // To know if node accept childs
    need_parent_config = need_parent;    // To know if node accept parent
    node_rank = rank;
    node_callback = (fct_ptr)callback;
    LOG_INFO("Node setup ok\n");
}

void setup_subgateway(uint8_t type, uint8_t rank, bool accept_child, void* callback){
    setup_gateway(type, rank, accept_child, true, callback);
}


void setup_node(uint8_t type, uint8_t rank, void* callback) {
    setup_subgateway(type, rank, false, callback);
}