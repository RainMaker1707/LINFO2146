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


list_t* childs = NULL;
mote_t* parent = NULL;
list_t* neighbors = NULL;

static uint8_t buffer[PACKET_SIZE];


bool accept_childs_config = false; // By default mote doesn't accept child

// TODO: Wait for DIS+ACK responses
// TODO: Add DIS+ACK responder motes in the neighbor_list
// TODO: Add an ALIVE system to remove node when not heard from a certain time threshold

// TODO: Unicast

// TODO: Broadcast to all
// TODO: broadcast to childs

// TODO: ADD_NODE: 
    // Send PRT to parent
    // If receive PRT+ACK add the mote has parent and return true
    // If receive PRT+NACK refused, so just return false



// ####################### API ############################

void unicast_unreliable_send(uint8_t* buffer){
    printf("UNICAST: %s\n", buffer);
}


void broadcast_unreliable_send(uint8_t* buffer){
    // TODO: if childs == NULL send to NULLNET(NULL) else send unicast to every child
    //uint8_t *encoded_data = (uint8_t*)encode(packet);
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    LOG_INFO("BUFFER PAYLOAD: %s\n", decode((char*)buffer)->payload);
    LOG_INFO("Broadcast packet %s\n", decode((char*)nullnet_buf)->payload); // Verify using nullnet_buf
    NETSTACK_NETWORK.output(NULL);
}


void multicast_unreliable_send(uint8_t* buffer){
    printf("MULTICAST: %s\n", buffer);
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
    LOG_INFO("PACKET PAYLOAD: %s\n", packet->payload);
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
            exit(1);
    }
}


void unreliable_wait_receive(char * buffer, void* callback){

}


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
    if(mote->signal_strenght > x) {
        printf("\033[1;31mNo mote with signal strength under %d\nMote returned is not contrained\033[0m\n", x);
    }
    return mote;
}


bool attach_parent(mote_t* mote){
    if(parent != NULL) { return false; }
    parent = mote;
    return true;
}


bool attach_to_tree(){
    int old_max = 10000;
    bool attached = false;
    if(neighbors->head == NULL) return false;
    while(!attached){
        mote_t* mote = search_max_under_x(neighbors, old_max);
        attached = attach_parent(mote);
        if(!attached) old_max = mote->signal_strenght;
    }
    return attached;
}


void discover_neighbor(){
    // Send DIS
    packet_t* packet = create_packet(DIS, 0, "aaaaaaaaaaaaaaaaa", NULL, "DISCOVER!");
    unreliable_send(packet, BROADCAST);
    free(packet);
}


/*
    Broadcast DIS
    Create neighbors list
    Choose a parent with appropriate rank and maximum signal strength, then initialise parent
    Create the child_list (empty at first by can add child if chosen as parent)
    @Param: accept_childs: if true then the mote will accept child when receive a PRT packet
                            by default set as false
*/
void setup_custom_node(bool accept_child){
    // NULLNET config
    nullnet_buf = buffer;
    nullnet_len = PACKET_SIZE;
    // SETUP inner list
    if(accept_child) childs = create_list(); // empty at first, only add when asked to be parent and accept
    neighbors = create_list();
    // DIS SEND
    discover_neighbor();
    accept_childs_config = accept_child; // To know if the node accept childs
    printf("Node setup ok\n");
}


void setup_node() {
    // setup NULLNET
    setup_custom_node(false);
}


void end_net(){
    free_list(neighbors);
    if(accept_childs_config) free(childs);
    free(parent);
}