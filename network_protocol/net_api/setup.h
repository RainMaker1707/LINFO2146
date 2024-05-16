#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"


#include "utils/packet.h"
#include "utils/net_list.h"


#include "sender.h"
#include "receiver.h"


typedef void (*fct_ptr)(packet_t* packet);



/*
*/
uint8_t get_rank();
uint8_t get_type();
mote_t* get_parent();
void set_parent(mote_t* mote);


/*
*/
fct_ptr get_callback();

list_t* get_neighbors();



// ############################### SETUP API ###############################

/*
    Broadast DIS
    Create neighbors list
    Choose a parent with appropriate rank and maximum signal strength, then initialise parent
    Create the child_list (empty at first by can add child if chosen as parent)
    @Param: accept_childs: if true then the mote will accept child when receive a PRT packet
                            by default set as false
*/
void setup_gateway(uint8_t type, uint8_t rank, bool accept_child, bool need_parent, void* callback);

void setup_subgateway(uint8_t type, uint8_t rank, bool accept_child, void* callback);


void setup_node(uint8_t type, uint8_t rank, void* callback);