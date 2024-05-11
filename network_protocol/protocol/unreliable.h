#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "../utils/packet.h"
#include "../utils/net_list.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2

#define PACKET_MAX_LEN 48           // packet are 48bytes max
#define HEADER_LEN 4                // header is 4 bytes
#define PACKET_PAYLOAD_MAX_LEN 12   // Payload is 12 bytes max

#define ALIVE_MAX_INTERVAL 15

#define GATEWAY 2
#define SUBGATEWAY 1
#define SENSOR 0


list_t* childs = NULL;
mote_t* parent = NULL;
list_t* neighbors = NULL;

static uint8_t buffer[PACKET_SIZE];      


typedef void (*fct_ptr)(packet_t* packet);

fct_ptr node_callback;

bool accept_childs_config = false; // By default mote doesn't accept child
bool need_parent_config = true; // By default mote accept parent;
uint8_t node_rank = 2; // By default mote is a sensor (node)



// TODO: Unicast

// TODO: Broadcast to all
// TODO: broadcast to childs

// TODO: ADD_NODE: 
    // Send PRT to parent
    // If receive PRT+ACK add the mote has parent and return true
    // If receive PRT+NACK refused, so just return false





// ##################### SENDER ########################


void unicast_unreliable_send(uint8_t* buffer, linkaddr_t* dst){
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    NETSTACK_NETWORK.output(dst);
}


void broadcast_unreliable_send(uint8_t* buffer){
    // TODO: if childs == NULL send to NULLNET(NULL) else send unicast to every child
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    NETSTACK_NETWORK.output(NULL);
}


void multicast_unreliable_send(uint8_t* buffer){
    printf("NOT IMPLEMENTED MULTICAST: %s\n", decode((char*)buffer)->payload);
}


/*
    Encode and send the packet to the address (mode=UNICAST), group address (mode=MULTICAST)
     or all address (mode=BROADCAST)
    Unreliable mean you will maybe not have an answer, and no packet will be acknowledged
    @Param: packet_t* packet: the well formated packet to encode and send
    @Param: int mode: the mode constant representing the sending mode
    @Param: char* adress: the adress or group adress used to send the packet (can be NULL for broadcast only)
*/
void unreliable_send(packet_t* packet, int mode){
    uint8_t* buffer = (uint8_t*) encode(packet);
    switch(mode){
        case UNICAST: 
            unicast_unreliable_send(buffer, packet->dst);
            break;
        case BROADCAST: 
            broadcast_unreliable_send(buffer);
            break;
        case MULTICAST: 
            multicast_unreliable_send(buffer);
            break;
        default:
            printf("\033[33mMode not compatible %d\033[0m\n", mode);
            return;
    }
    free(buffer);
}








// ####################### API ############################


mote_t* search_max_under_x(list_t* neighbors, int x){
    neighbors->current = neighbors->head->next;
    int max = neighbors->head->mote->signal_strenght;
    mote_t* mote = neighbors->head->mote;
    while(neighbors->current != neighbors->head){
        if(neighbors->current->mote->signal_strenght > max && neighbors->current->mote->signal_strenght < x){
            max = neighbors->current->mote->signal_strenght;
            mote = neighbors->current->mote;
        }
        neighbors->current = neighbors->current->next;
    }
    if(mote->signal_strenght > x) return NULL;
    return mote;
}

// TEST print mote lsit
void print_neighbors(){
    if(neighbors->head != NULL){
        neighbors->current = neighbors->head;
        while(neighbors->current != neighbors->tail){
            LOG_INFO("NEIGHBORS: ");
            LOG_INFO_LLADDR(neighbors->current->mote->adress);
            LOG_INFO("\n");
            neighbors->current = neighbors->current->next;
        }
        LOG_INFO("NEIGHBORS: ");
        LOG_INFO_LLADDR(neighbors->tail->mote->adress);
        LOG_INFO("\n");
    }
}

node_t* find_neighbor_in_list(list_t* list, const linkaddr_t* address){
    if(list == NULL || list->head == NULL) return NULL; 
    list->current = list->head;
    while(1){
        if(linkaddr_cmp(list->current->mote->adress, address)) return list->current;
        if(list->current == list->tail) return NULL;
        list->current =list->current->next;
    }
}




// ################## PRT API ################

void send_prt(const linkaddr_t* src){
    if(need_parent_config){
        packet_t* packet = create_packet(PRT, node_rank, (const linkaddr_t*)&linkaddr_node_addr, src, "PARENT");
        unreliable_send(packet, UNICAST);
        free_packet(packet);
        LOG_INFO("PRT sent ");
        LOG_INFO_LLADDR(src);
        LOG_INFO("\n");
    }
}

void prt_received(packet_t* packet, const linkaddr_t* src){
    add_child(childs, find_neighbor_in_list(neighbors, src)->mote);
    LOG_INFO("CHILD accepted -> ");
    LOG_INFO_LLADDR(src);
    LOG_INFO("\n");
}


void attach_parent(packet_t* packet, const linkaddr_t* src){
    // TODO: check signal strength
    if(need_parent_config && parent == NULL && packet->src_rank == node_rank+1) {
        if(find_neighbor_in_list(neighbors, packet->src) == NULL) LOG_INFO("PARENT NOT FIND IN NEIGHBOR\n");
        else if(linkaddr_cmp((const linkaddr_t*)packet->src, &linkaddr_null)) LOG_INFO("Cannot attach NULL as parent\n");
        else {
            send_prt(packet->src);
            parent = find_neighbor_in_list(neighbors, packet->src)->mote;
        }
    }
}




// ################ DISCOVER API ##########################

void discover_neighbor(){
    // Send DIS
    packet_t* packet = create_packet(DIS, node_rank, (const linkaddr_t*)&linkaddr_node_addr, NULL, "DISCOVER!");
    unreliable_send(packet, BROADCAST);
    free_packet(packet);
}

void receive_dis(packet_t* packet, const linkaddr_t* src){
    if(!list_contains_src(neighbors, (linkaddr_t*)packet->src)) {
        mote_t* mote = create_mote(packet->src_rank, packet->src, 10); // TODO: replace 10 by signal strength
        add_child(neighbors, mote);
        LOG_INFO("Add mote in neighbors list\n");
    }
    if(packet->flags == DIS){
        // Respond to DIS but not DIS+ACK
        LOG_INFO("DIS RECEIVED!\n");
        packet_t* packet = create_packet(DIS+ACK, node_rank, (const linkaddr_t*)&linkaddr_node_addr, src, "DIS+ACK");
        unreliable_send(packet, UNICAST);
        free_packet(packet);
    }else if(packet->flags == DIS+ACK) LOG_INFO("DIS+ACK received!\n");
}






// ############### ALIVE API ################

/*
  set the last time heard parameter of the neighbor to the current cpu time
*/
void neighbor_is_alive(packet_t* packet, const linkaddr_t* neigh_address){
    if(neighbors == NULL || neigh_address == NULL) return;
    node_t* neighbor = find_neighbor_in_list(neighbors, neigh_address);
    if(neighbor == NULL){
        // Do something if not in the list like send a DIS ?
        packet_t* packet = create_packet(DIS, node_rank, (const linkaddr_t*)&linkaddr_node_addr, neigh_address, "DISCOVER!");
        unreliable_send(packet, BROADCAST);
        free_packet(packet);
        printf("Neighbor is not in the list\n");
        return;
    }
    printf("setting time for neighbor\n");
    neighbor->mote->last_time_heard = clock_time();
}



/*
 function that loops through the list of neighbors and check if the last time heard is too old

*/
void process_neighbors_last_time(list_t* list){
    if(list == NULL||list->head == NULL) return;
    unsigned long long alive_difference = 20 * CLOCK_SECOND;

    node_t* tmp = list->current;
    list->current = list->head;
    while(1){
        if(clock_time() - list->current->mote->last_time_heard > alive_difference){
            if(parent == list->current->mote) parent=NULL;
            if(list->head==list->tail){
                free(list->current->mote);
                free(list->current);
                list->head = NULL;
                list->tail = NULL;
                list->current = NULL;
                return;
            }
            if(list->current == list->head){
                list->head = list->current->next;
                free(list->current->mote);
                free(list->current);
                list->current = list->head;
                list->tail->next = list->head;
            } else if(list->current == list->tail){
                tmp->next = list->head;
                free(list->current->mote);
                free(list->current);
                list->tail = tmp;
            }else{
                tmp->next = list->current->next;
                free(list->current->mote);
                free(list->current);
                list->current = tmp;
            }

        }
        if(list->current == list->tail) return;
        tmp = list->current;
        list->current = list->current->next;
    }
}

void check_neighbors_last_time_heard(){
    process_neighbors_last_time(neighbors);
}

void heartbeat(){
        packet_t *packet = create_packet(DIO, node_rank, &linkaddr_node_addr, NULL, "alive");
        unreliable_send(packet, BROADCAST);
        free_packet(packet);
}









// #################### RECEIVER #######################

void unreliable_wait_receive(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
    char encoded[PACKET_SIZE];
    memcpy(&encoded, data, PACKET_SIZE);
    packet_t* packet = decode((char*)&encoded);
    if(packet == NULL) return;
    if(!linkaddr_cmp(packet->dst, &linkaddr_node_addr) && !linkaddr_cmp(packet->dst, &linkaddr_null)){
        // redistribute to parent if no child, or to child if have one that is the correct one else (if no parent and no child corresponding)
        LOG_INFO("DISCARD\n");
        return;
    }
    LOG_INFO("Received %s from ", packet->payload);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
    
    switch(packet->flags){
        case UDP:
            // GIVE PACKET TO USER (MOTE).
            node_callback(packet);
            break;
        case PRT:       // ASK FOR PARENTING
            LOG_INFO("PRT received");
            prt_received(packet, src);
            break;
        case DIO:       // ALIVE SYSTEM
            neighbor_is_alive(packet, src);
            attach_parent(packet, src);
			break;
        case DIS:
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        case DIS+ACK:
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        default:
            LOG_INFO("No flags recognized %d\n", packet->flags);
            break;
    }
    free(packet);
}






// ############################### SETUP API ###############################

/*
    Broadcast DIS
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
    // SETUP inner list
    if(accept_child) childs = create_list(); // empty at first, only add when asked to be parent and accept
    neighbors = create_list();
    accept_childs_config = accept_child; // To know if node accept childs
    need_parent_config = need_parent;    // To know if node accept parent
    node_rank = rank;
    node_callback = (fct_ptr)callback;
    nullnet_set_input_callback(unreliable_wait_receive);
    LOG_INFO("Node setup ok\n");
}

void setup_subgateway(uint8_t rank, bool accept_child, void* callback){
    setup_gateway(rank, accept_child, true, callback);
}


void setup_node(uint8_t rank, void* callback) {
    setup_subgateway(rank, false, callback);
}


void end_net(){
    free_list(neighbors);
    if(accept_childs_config) free(childs);
    free(parent);
}