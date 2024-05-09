#include "unreliable.h"



void unicast_unreliable_send(char* buffer, char* adress){

}


void broadcast_unreliable_send(char* buffer){
    // if childs == NULL send to NULLNET(NULL)
    // else send unicast to every child
}

void multicast_unreliable_send(char* buffer, char* group_address){

}


void unreliable_send(packet_t* packet, int mode, char* address){
    char* buffer = encode(packet);
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


void attach_to_tree(list_t* neighbors){
    int old_max = INT32_MAX;
    bool attached = false;
    while(!attached){
        mote_t* mote = search_max_under_x(neighbors, old_max);
        attached = attach_parent(mote);
        if(!attached) old_max = mote->signal_strenght;
    }
}


list_t* discover_neighbor(){
    list_t* neighbors = create_list();
    // Send DIS
    packet_t packet;
    packet.checksum = 0xff;
    packet.flags = DIS;
    packet.packet_number = 0;
    packet.version = VERSION;
    packet.src = "aaaaaaaaaaaaaaaaa";
    packet.dst = NULL;
    packet.payload = "Hello UDP :)";
    unreliable_send(&packet, BROADCAST, NULL);
    // Wait for DIS+ACK responses
    // Add DIS+ACK responder motes in the neighbor_list
    return neighbors;
}

bool attach_parent(mote_t* mote){
    if(parent != NULL) { return false; }
    // Send PRT to parent
    // If receive PRT+ACK add the mote has parent and return true
    // If receive PRT+NACK refused, so just return false
    return false;
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


    list_t* list = create_list();
    mote_t* mote1 = create_mote(1, "aaaaaaaaaaaaaaaaa", 11);
    mote_t* mote2 = create_mote(1, "bbbbbbbbbbbbbbbbb", 13);
    mote_t* mote3 = create_mote(1, "ccccccccccccccccc", 12);
    add_child(list, mote1);
    add_child(list, mote2);
    add_child(list, mote3);

    int x = 15;
    printf("MAX under %d -> mote strenght: %d\n", x, search_max_under_x(list, x)->signal_strenght);
    printf("Last time heard mote1: %llu\n", mote1->last_time_heard);

    printf("Unreliable protocol stop\n");
    exit(0);
}