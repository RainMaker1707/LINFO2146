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


typedef void (*fct_ptr)(packet_t* packet);

fct_ptr node_callback;

bool accept_childs_config = false; // By default mote doesn't accept child



// TODO: Add an ALIVE system to remove node when not heard from a certain time threshold
// TODO: Unicast

// TODO: Broadcast to all
// TODO: broadcast to childs

// TODO: ADD_NODE: 
    // Send PRT to parent
    // If receive PRT+ACK add the mote has parent and return true
    // If receive PRT+NACK refused, so just return false



// ####################### API ############################


void unicast_unreliable_send(uint8_t* buffer, linkaddr_t* dst){
    printf("UNICAST\n");
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    LOG_INFO("BUFFER PAYLOAD: %s\n", decode((char*)buffer)->payload);
    LOG_INFO("Broadcast packet %s\n", decode((char*)nullnet_buf)->payload); // Verify using nullnet_buf
    NETSTACK_NETWORK.output(dst);
}


void broadcast_unreliable_send(uint8_t* buffer){
    // TODO: if childs == NULL send to NULLNET(NULL) else send unicast to every child
    printf("BROADCAST\n");
    memcpy(nullnet_buf, buffer, PACKET_SIZE);  // Use nullnet_buf directly
    LOG_INFO("BUFFER PAYLOAD: %s\n", decode((char*)buffer)->payload);
    LOG_INFO("Broadcast packet %s\n", decode((char*)nullnet_buf)->payload); // Verify using nullnet_buf
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
    LOG_INFO("PACKET PAYLOAD: %s\n", packet->payload);
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
            exit(1);
    }
}


void receive_dis(packet_t* packet, const linkaddr_t* src){
    mote_t* mote = create_mote(0, packet->src, 10);
    // TODO check if addr is not already in neighbors
    add_child(neighbors, mote);
    LOG_INFO("Add mote in neighbors list\n");
    if(packet->flags == DIS){
        // Respond to DIS but not DIS+ACK
        LOG_INFO("DIS RECEIVED!\n");
        packet_t* packet = create_packet(DIS+ACK, 1, (const linkaddr_t*)&linkaddr_node_addr, src, "DIS+ACK");
        unreliable_send(packet, UNICAST);
        free(packet);
    }else LOG_INFO("DIS+ACK received!\n");

    // TEST print mote lsit
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


void unreliable_wait_receive(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
    LOG_INFO("RECEIVED PACKET of len %d!\n", len);
    char encoded[PACKET_SIZE];
    memcpy(&encoded, data, PACKET_SIZE);

    packet_t* packet = decode((char*)&encoded);
    if(packet == NULL) {
        LOG_INFO("Error decoding\n");
        return;
    }
    LOG_INFO("Received %s from ", packet->payload);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
    
    switch(packet->flags){
        case DIS:
            receive_dis(packet, src);
            break;
        case DIS+ACK:
            receive_dis(packet, src);
            break;
        case UDP:
                break;
        default:
            LOG_INFO("No flags recognized %d\n", packet->flags);
            break;
    }
    node_callback(packet);
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
    packet_t* packet = create_packet(DIS, 0, (const linkaddr_t*)&linkaddr_node_addr, NULL, "DISCOVER!");
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
void setup_custom_node(bool accept_child, void* callback){
    // NULLNET config
    nullnet_buf = buffer;
    nullnet_len = PACKET_SIZE;
    // SETUP inner list
    if(accept_child) childs = create_list(); // empty at first, only add when asked to be parent and accept
    neighbors = create_list();
    // DIS SEND
    discover_neighbor();
    accept_childs_config = accept_child; // To know if the node accept childs
    node_callback = (fct_ptr)callback;
    nullnet_set_input_callback(unreliable_wait_receive);
    LOG_INFO("Node setup ok\n");
}


void setup_node(void* callback) {
    // setup NULLNET
    setup_custom_node(false, callback);
}


void end_net(){
    free_list(neighbors);
    if(accept_childs_config) free(childs);
    free(parent);
}