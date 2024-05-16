#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "../utils/packet.h"
#include "../utils/net_list.h"
#include "random.h"


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UNICAST 0
#define BROADCAST 1
#define MULTICAST 2

#define PACKET_MAX_LEN PACKET_SIZE           // packet are 32 bytes max

#define ALIVE_MAX_INTERVAL 15

#define GATEWAY 2
#define SUBGATEWAY 1
#define SENSOR 0
#define GATEWAY_ADDR {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}

/*define device types*/
#define LIGHT_SENSOR 3
#define IRRIGATION 4
#define LIGHT_BULB 5
#define MOBILE 6


list_t* childs = NULL;
mote_t* parent = NULL;
list_t* neighbors = NULL;

static uint8_t buffer[PACKET_SIZE];      


typedef void (*fct_ptr)(packet_t* packet);

fct_ptr node_callback;

bool accept_childs_config = false; // By default mote doesn't accept child
bool need_parent_config = true; // By default mote accept parent;
uint8_t node_rank = 2; // By default mote is a sensor (node)

int device_type = -1 ;

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
        LOG_INFO(" dev type: %d", current->mote->device_type);
        LOG_INFO("\n");
        LOG_INFO("%d: SRC: ", counter++);
        LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->src));
        LOG_INFO("\n");
        if(current == neighbors->tail) return;
        current = current->next;
    }
}

void log_packet_for_server(packet_t* packet){
    LOG_INFO("Server log\n");
    LOG_INFO("Src: ");
    LOG_INFO_LLADDR(packet->src);
    LOG_INFO("\n");
    LOG_INFO("Dst: ");
    LOG_INFO_LLADDR(packet->dst);
    LOG_INFO("\n");
    LOG_INFO("Payload: %s\n", packet->payload);
}

bool contains_rly(packet_t* packet){
    uint8_t* flags = retrieve_flags(packet);
    // flags == {TCP, MLT, ACk, NACK, RLY, DIO, DIS, PRT}
    return flags[4] == 1;
}

/* call set device type when starting mote */
void set_device_type(int type){
    device_type = type;
}


// ##################### SENDER ########################


void unicast_unreliable_send(uint8_t* buffer, const linkaddr_t* dst){
    //nullnet_len = PACKET_SIZE;
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    // Copy dest in a new linkaddr_t to avoid rewritting pointer
    linkaddr_t dest;
    linkaddr_copy((linkaddr_t*)&dest, (const linkaddr_t*)&(find_neighbor_in_list(neighbors, dst)->mote->src));
    NETSTACK_NETWORK.output(&dest);
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


void send_light(){
    static linkaddr_t dest_addr = GATEWAY_ADDR;   
    char* payload = malloc(12*sizeof(char));
    sprintf(payload, "%d", random_rand()%101);
    packet_t* packet = create_packet(UDP, node_rank, (const linkaddr_t*)&linkaddr_node_addr, (const linkaddr_t*)&dest_addr, payload);
    unreliable_send(packet, BROADCAST);
    free_packet(packet);
    free(payload);
}


/*
function for the mobile terminal that sends z maintenance messages to the light sensors
*/
void send_maintenance(){
    if(neighbors->head == NULL) {
        LOG_INFO("## NO NEIGHBORS\n");
        return;
    }
    node_t* current = neighbors->head;
    /* check if unknown devices */
    while(1){
        if(current->mote->device_type == -1){
            LOG_INFO("neighbors setup not complete yet\n");
            free(current);
            return;
        }
        if(current == neighbors->tail) {
            free(current);
            break;
        }
        current = current->next;
    }
    current = neighbors->head;
    /* start sending maintenance messages*/
    while(1){
        if(current->mote->device_type == LIGHT_SENSOR){
            int z = 3; // Number of messages to send to each sensor
            for (int i = 0; i < z; i++){
                printf("maintenance message %d for ", i);
                LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->adress));
                printf("\n");
                 packet_t* packet = create_packet(TCP, node_rank, (const linkaddr_t*)&linkaddr_node_addr, (const linkaddr_t*)&(current->mote->adress), "mtnce");
                unreliable_send(packet, UNICAST);
                free_packet(packet);
            }      
        }
        if(current == neighbors->tail){
            free(current);
            break;
        }
        current = current->next;
    }
}

void ack_maintenance(packet_t* packet){
    packet_t* ack_packet = create_packet(TCP+ACK, node_rank, (const linkaddr_t*)&linkaddr_node_addr, packet->src, "ACK mtnce");
    unreliable_send(ack_packet, UNICAST);
    free_packet(ack_packet);
}

/* loop through neighbors and send unicast packet to them */
void multicast_send(packet_t* packet ,int type, char* payload){
    node_t* current = neighbors->head;
    while(1){
        if(current->mote->device_type == type){
            printf("send mult\n");
            LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->adress));
            printf(" - ");
            LOG_INFO_LLADDR(packet->src);
            printf("\n");
            packet_t* packet_mlt = create_packet(UDP, node_rank, packet->src, (const linkaddr_t*)&(current->mote->adress), "a");
            unreliable_send(packet_mlt, UNICAST);
            free_packet(packet_mlt);
        }else{
            printf("not send mult\n");
            printf(" curr = %d and type %d\n", current->mote->device_type, type);
        }
        if(current == neighbors->tail) break;
        current = current->next;
    }
    free(current);
}

void multicast_message(packet_t* packet){
    // check multicast payload to send message to corresponding sensors
    if(strcmp(packet->payload, "light") == 0){ 
        printf("multicast message to light bulb\n");
        multicast_send(packet, LIGHT_BULB, "light"); 
        return;
    }
    else if(strcmp(packet->payload, "irrigation") == 0){
        printf("multicast message to irrigation\n");
        multicast_send(packet, IRRIGATION, "irrigation");
    } 
}




// ################## PRT API ################

void send_prt(const linkaddr_t* src){
    if(need_parent_config){
        packet_t* packet = create_packet(PRT, node_rank, (const linkaddr_t*)&linkaddr_node_addr, src, "PARENT");
        unreliable_send(packet, UNICAST);
        free_packet(packet);
    }
}

void prt_received(packet_t* packet, const linkaddr_t* src){
    add_child(childs, find_neighbor_in_list(neighbors, src)->mote);
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
        mote_t* mote = create_mote(packet->src_rank, packet->src, 10, src); // TODO: replace 10 by signal strength
        add_child(neighbors, mote);
    }
    // Respond to DIS but not DIS+ACK
    if(packet->flags == DIS){ 
        packet_t* packet_sd = create_packet(DIS+ACK, node_rank, (const linkaddr_t*)&linkaddr_node_addr, packet->src, "DIS+ACK");
        unreliable_send(packet_sd, UNICAST);
        free_packet(packet_sd);
    }
}






// ############### ALIVE API ################

/*
  set the last time heard parameter of the neighbor to the current cpu time
*/
void neighbor_is_alive(packet_t* packet, const linkaddr_t* neigh_address){
    if(neighbors == NULL || neigh_address == NULL || packet == NULL) return;
    node_t* neighbor = find_neighbor_in_list(neighbors, packet->src);
    if(neighbor == NULL){
        discover_neighbor();
        LOG_INFO(" neighbor not found ");
        LOG_INFO_LLADDR(packet->src);
        LOG_INFO("\n");
        return;
    }
    if(neighbor->mote->device_type == -1){
        neighbor->mote->device_type = atoi(packet->payload);
    }
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
    char* payload = malloc(12*sizeof(char));
    sprintf(payload, "%d", device_type);
    packet_t *packet = create_packet(DIO, node_rank, &linkaddr_node_addr, NULL, payload);
    unreliable_send(packet, BROADCAST);
    free_packet(packet);
    free(payload);
}









// #################### RECEIVER #######################


void switch_responder(packet_t* packet, const linkaddr_t* src, const linkaddr_t* dest){
    // Remove RLY flag if contained
    if(contains_rly(packet)) {
        packet->flags -= RLY;
    }
    if(node_rank == GATEWAY){ // Current gateway rank, TODO add pkt flag filter like && packet->flags == UDP
        //log_packet_for_server(packet);
    }

    switch(packet->flags){
        case UDP:
            // GIVE PACKET TO USER (MOTE).
            LOG_INFO("UDP received\n");

            /* LOG TO THE SERVER (do not delete)*/
            if(node_rank == GATEWAY){ 
                LOG_INFO("Server log");
                LOG_INFO("light: ");
                LOG_INFO_LLADDR(packet->src);
                LOG_INFO("from: ");
                LOG_INFO_LLADDR(src);
                LOG_INFO("Payload: %s\n", packet->payload);
                LOG_INFO("\n");
            }

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
        case DIS:       // DISCOVER SYSTEM
            LOG_INFO("DIS received\n");
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        case DIS+ACK:   // DISCOVER SYSTEM
            LOG_INFO("DIS+ACK received\n");
            receive_dis(packet, src);
            attach_parent(packet, src);
            break;
        case TCP:       // Maintenance messages
            LOG_INFO("TCP received\n");
            ack_maintenance(packet);
            break;
        case TCP+ACK:   // Ack maintenance
            LOG_INFO("TCP+ACK received from ");
            LOG_INFO_LLADDR(packet->src);
            LOG_INFO("\n");
            break;
        case MLT:       // Multicast from server
            LOG_INFO("MLT received\n");
            multicast_message(packet);
            break;
        default:
            LOG_INFO("No flags recognized %d\n", packet->flags);
            break;
    }
}

bool received_from_child(packet_t* packet, const linkaddr_t* src){
    if(!accept_childs_config) return false;
    return list_contains_src(childs, (linkaddr_t*)src);
}

void send_to_parent(packet_t* packet){
    unreliable_send(packet, UNICAST);
}

void send_to_childs(packet_t* packet, const linkaddr_t* src){
    node_t* current = childs->head;
    while(1){
        linkaddr_copy(packet->dst, &current->mote->adress);
        if(!linkaddr_cmp((const linkaddr_t*)&(current->mote->adress), src)) unreliable_send(packet, UNICAST);
        if(current == childs->tail) break;
        current = current->next;
    }
}

void broadcast_retransmition(packet_t* packet, const linkaddr_t* src){
    // TODO: broadcast retransmit algorithm correction
    // IF no parent and no childs doesn't retransmit
    if(parent == NULL && (!accept_childs_config || childs->head == NULL)) return;
    packet_t* packet_rly;
    if(contains_rly(packet)) packet_rly = create_packet(packet->flags, node_rank, packet->src, packet->dst, packet->payload);
    else packet_rly = create_packet(packet->flags+RLY, node_rank, packet->src, packet->dst, packet->payload);

    // If comes to parent
    if(parent != NULL && linkaddr_cmp(src, (const linkaddr_t*)&(parent->adress)) && accept_childs_config && childs->head != NULL){
        send_to_childs(packet_rly, src);
    // If comes from a child
    }else if (list_contains_src(childs, (linkaddr_t*)src)){
        if(parent != NULL){
            linkaddr_copy(packet_rly->dst,  &parent->adress);
            unreliable_send(packet_rly, UNICAST);
        }


        if (node_rank != GATEWAY) {
            send_to_childs(packet_rly, src);
        }
        
        
    }
    free_packet(packet_rly);
}


void unreliable_wait_receive(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
    char encoded[PACKET_SIZE];
    memcpy(&encoded, data, PACKET_SIZE);
    packet_t* packet = decode((char*)&encoded);
    // Discard packet IF no packet or packet firstly sent by this node or the sender node has same rank as the receiver one
    if(packet == NULL || linkaddr_cmp(packet->src, &linkaddr_node_addr) || linkaddr_cmp(src, &linkaddr_node_addr) || packet->src_rank == node_rank) {
        printf("Packet discarded\n");
        free_packet(packet);
        return;
    }
    if(!linkaddr_cmp(packet->dst, &linkaddr_node_addr) && !linkaddr_cmp(packet->dst, &linkaddr_null)){
        // relay UNICAST packet to dest
        printf("Relay packet\n");
        packet->src_rank = node_rank;
        unreliable_send(packet, UNICAST);
        return;
    }
    if(linkaddr_cmp(packet->dst, &linkaddr_node_addr)){
        printf("Packet for this node\n");
        // Packet is for this node
    }
    if(linkaddr_cmp(packet->dst, &linkaddr_null) || contains_rly(packet)){
        // retransmit following broadcast algo
        broadcast_retransmition(packet, src);
    }
    switch_responder(packet, src, dest);
    free_packet(packet);
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
    clock_wait(50);
    discover_neighbor();
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
    if(accept_childs_config) free_list(childs);
    free(parent);
}


/*
function for the gateway to handle the server messages (called in gateway_node.c)
*/
void handle_server_response(char* data){
    LOG_INFO("Server response: %s\n", data);

    /* server sent a light start message*/
    /*
    current server_test.py response is "l address" like "l 0200000000000000"
    and the address is the one of the subgateway that provided the light sensor message to the gateway

    */
    if (*data == 'l'){ 
        

        //transform char to address
        char* str_addr = malloc(strlen(data)*sizeof(char));
        for(int i=2; i<strlen(data); i++){
            str_addr[i-2] = data[i];
        }

        linkaddr_t* addr = char_to_linkaddr(str_addr);
        if(addr == NULL) return;

        //create a packet type MLT and send it to the subgateway (addr)
        packet_t* packet = create_packet(MLT, node_rank, (const linkaddr_t*)&linkaddr_node_addr, addr, "light");
        unreliable_send(packet, UNICAST);
        free_packet(packet);

        free(str_addr);
    }

    /* irrigation message from server */
    if (*data == 'i'){

        LOG_INFO("Server sent irigation message\n");
        //TODO implem

        /* idea :*/
        node_t* current = neighbors->head;
        while(1){
            if(current->mote->device_type==SUBGATEWAY){
                packet_t* packet = create_packet(MLT, node_rank, (const linkaddr_t*)&linkaddr_node_addr, (const linkaddr_t*)&(current->mote->adress), "irrigation");
                unreliable_send(packet, UNICAST);
                free_packet(packet);
            }
            if(current == neighbors->tail) break;
            current = current->next;
        }
    }
}