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

#define PACKET_MAX_LEN PACKET_SIZE           // packet are 32 bytes max
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






// ####################### API ############################

node_t* find_neighbor_in_list(list_t* list, const linkaddr_t* address){
    if(list == NULL || list->head == NULL) return NULL; 
    node_t* current = list->head;
    while(1){
        if(linkaddr_cmp(&(current->mote->adress), address)) return current;
        if(current == list->tail) return NULL;
        current = current->next;
    }
}

void print_table(){
    if(neighbors->head == NULL) {
        LOG_INFO("## NO NEIGHBORS\n");
        return;
    }
    int counter = 0;
    node_t* current = neighbors->head;
    while(1){
        LOG_INFO("%d: ADD: ", counter);
        LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->adress));
        LOG_INFO("\n");
        LOG_INFO("%d: SRC: ", counter++);
        LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->src));
        LOG_INFO("\n");
        if(current == neighbors->tail) return;
        current = current->next;
    }
}

void log_packet_for_server(packet_t* packet){
    LOG_INFO(" Server log ");
    LOG_INFO("Src: ");
    LOG_INFO_LLADDR((linkaddr_t*)&(packet->src));
    LOG_INFO("Dst: ");
    LOG_INFO_LLADDR((linkaddr_t*)&(packet->dst));
    LOG_INFO("Payload: %s\n", packet->payload);
}




// ##################### SENDER ########################


void unicast_unreliable_send(uint8_t* buffer){
    nullnet_len = PACKET_SIZE;
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    int start = clock_time();
    while(clock_time() - start <= 10){continue;}
    NETSTACK_NETWORK.output(NULL);
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
            unicast_unreliable_send(buffer);
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
    LOG_INFO("From ");
    LOG_INFO_LLADDR(src);
    LOG_INFO("\n");

    if(!list_contains_src(neighbors, (linkaddr_t*)packet->src)) {
        mote_t* mote = create_mote(packet->src_rank, packet->src, 10, src); // TODO: replace 10 by signal strength
        add_child(neighbors, mote);
        LOG_INFO("Add mote in neighbors list\n");
    }
    // Respond to DIS but not DIS+ACK
    if(packet->flags == DIS){ 
        packet_t* packet_sd = create_packet(DIS+ACK, node_rank, (const linkaddr_t*)&linkaddr_node_addr, src, "DIS+ACK");
        unreliable_send(packet_sd, UNICAST);
        free_packet(packet_sd);
    }
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
    unsigned long long alive_difference = 60 * CLOCK_SECOND;

    
    node_t* current = list->head;
    node_t* tmp = current;
    while(1){
        if(clock_time() - current->mote->last_time_heard > alive_difference){
            if(parent == current->mote) parent=NULL;
            if(list->head==list->tail){
                free(current->mote);
                free(current);
                list->head = NULL;
                list->tail = NULL;
                current = NULL;
                return;
            }
            if(current == list->head){
                list->head = current->next;
                free(current->mote);
                free(current);
                current = list->head;
                list->tail->next = list->head;
            } else if(current == list->tail){
                tmp->next = list->head;
                free(current->mote);
                free(current);
                list->tail = tmp;
            }else{
                tmp->next = current->next;
                free(current->mote);
                free(current);
                current = tmp;
            }

        }
        if(current == list->tail) return;
        tmp = current;
        current = current->next;
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


void switch_responder(packet_t* packet, const linkaddr_t* src, const linkaddr_t* dest){

    if(node_rank==2){ // Current gateway rank, TODO change later to match the actual rank of the gateway
        log_packet_for_server(packet);
    }
    //log_packet_for_server(packet);

    switch(packet->flags){
        case UDP:
            // GIVE PACKET TO USER (MOTE).
            LOG_INFO("UDP received\n");
            node_callback(packet);
            break;
        case PRT:       // ASK FOR PARENTING
            LOG_INFO("PRT received\n");
            prt_received(packet, src);
            break;
        case DIO:       // ALIVE SYSTEM
            LOG_INFO("DIO received\n");
            neighbor_is_alive(packet, src);
            attach_parent(packet, src);
			break;
        case DIS:
            LOG_INFO("DIS received\n");
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        case DIS+ACK:
            LOG_INFO("DIS+ACK received\n");
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        default:
            LOG_INFO("No flags recognized %d\n", packet->flags);
            break;
    }
}

bool come_from_child(packet_t* packet){
    if(!accept_childs_config) return false;
    return list_contains_src(childs, packet->src);
}

void send_to_parent(packet_t* packet){
    unreliable_send(packet, UNICAST);
}

void broadcast_retransmition(packet_t* packet, const linkaddr_t* src){
    // TODO: broadcast retransmit algorithm correction
    // IF no parent and no childs doesn't retransmit
    // IF mote has same rank as this one, doesn't retransmit
    if((parent == NULL && (!accept_childs_config || childs->head == NULL))) return;
    else {
        // If not received from parent retransmit to parent
        LOG_INFO("Need to retransmit packet\n");
    }
}

void unreliable_wait_receive(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
    char encoded[PACKET_SIZE];
    memcpy(&encoded, data, PACKET_SIZE);
    packet_t* packet = decode((char*)&encoded);
    // Discard packet IF no packet or packet firstly sent by this node or the sender node has same rank as the receiver one
    if(packet == NULL || linkaddr_cmp(packet->src, &linkaddr_node_addr) || linkaddr_cmp(src, &linkaddr_node_addr) || packet->src_rank == node_rank) {
        free(packet);
        return;
    }
    if(!linkaddr_cmp(packet->dst, &linkaddr_node_addr) && !linkaddr_cmp(packet->dst, &linkaddr_null)){
        // redistribute to parent if no child, or to child if have one that is the correct one else (if no parent and no child corresponding)
        // UNICAST RETRANSMISSION
    }
    if(linkaddr_cmp(packet->dst, &linkaddr_null) && parent != NULL){
        // retransmit following broadcast algo
        broadcast_retransmition(packet, src);
    }
    switch_responder(packet, src, dest);
    // node_callback(packet);
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