#include "unreliable.h"



void unicast_unreliable_send(char* buffer, char* adress){

}


void broadcast_unreliable_send(char* buffer){

}

void multicast_unreliable_send(char* buffer, char* group_address){

}


void unreliable_send(packet_t* packet, int mode, char* address){
    //char* buffer = encode(packet);
    char* buffer;
    switch(mode){
        case UNICAST: 
            unicast_unreliable_send(buffer, address);
            break;
        case BROADCAST: 
            broadcast_unreliable_send(buffer);
            break;
        case MULTICAST: 
            multicast_unreliable_send(buffer, address);
            break;
        default:
            printf("\033[33mMode not compatible %d\033[0m\n", mode);
            exit(1);
    }
}


neighbor_list_t* discover_neighbor(){

}

void attach_parent(neighbor_list_t* neighbor_list){

}

child_list_t* create_child_list(){

}

void add_child(child_list_t* child_list){

}

void free_child_list(child_list_t* child_list){

}

void unreliable_wait_receive(char * buffer){

}


// ########################" END API ######################

// ONLY FOR TESTS PURPOSES
int main(){
    printf("Unreliable protocol start\n");

    packet_t packet;
    packet.checksum = 0xff;
    packet.flags = TCP+SYN+ACK;
    packet.packet_number = 0;
    packet.version = VERSION;
    packet.src = "aaaaaaaaaaaaaaaaa";
    packet.dst = "bbbbbbbbbbbbbbbbb";
    packet.payload = "Hello UDP :)";

    unreliable_send(&packet, UNICAST, packet.src);

    printf("Packet payload: %s\n", packet.payload);

    printf("Unreliable protocol stop\n");
    exit(0);
}