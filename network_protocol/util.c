#include "util.h"

packet_t* create_packet(uint8_t flags, uint8_t packet_number, char* source_ip, char* dest_ip, char* payload){
    packet_t* packet = malloc(sizeof(packet_t));
    packet->version = VERSION;
    packet->flags = flags;
    packet->packet_number = packet_number;
    packet->src = source_ip;
    packet->dst = dest_ip;
    packet->checksum = compute_checksum(packet); // TODO, for now 1byte at 1
    return packet;
}

bool check_packet(packet_t* packet){
    if(packet->checksum == compute_checksum(packet)){
        return true;
    }
    return false;
}

uint8_t compute_checksum(packet_t* packet){
    // TODO
    return 0xff;
}

reader_pt check_read_packet(packet_t* packet) {
    reader_t* toRet = (reader_t*) malloc(sizeof(reader_t));
    if(check_packet(packet)){
        toRet->checker = true;
        toRet->payload = packet->payload;
    } else {
        toRet->checker = false;
        toRet->payload = NULL;
    }
    return toRet;
}

char* encode(packet_t* packet){
    char* buffer = malloc(sizeof(char)*48);
    buffer[0] = VERSION;
    buffer[1] = packet->flags;
    buffer[2] = packet->packet_number;
    buffer[3] = packet->checksum;
    for(int i = 0; i<16; i++){
        buffer[4+i] = packet->src[i];
        buffer[20+i] = packet->dst[i];
    }
    for(int i = 0; i<12; i++){
        buffer[36+i] = packet->payload[i];
    }
    return buffer;
}

packet_t* decode(char* buffer){
    packet_t* packet = malloc(sizeof(packet_t));
    packet->version = buffer[0];
    packet->flags = buffer[1];
    packet->packet_number = buffer[2];
    packet->checksum = buffer[3];
    packet->src = malloc(16*sizeof(char));
    packet->dst = malloc(16*sizeof(char));
    for(int i = 0; i<16; i++){
        packet->src[i] = buffer[4+i];
        packet->dst[i] = buffer[20+i];
    }
    packet->payload = malloc(12*sizeof(char));
    for(int i = 0; i<12; i++){
        packet->payload[i] = buffer[36+i];
    }
    return packet;
}

void free_packet(packet_t* packet){
    free(packet->src);
    free(packet->dst);
    free(packet->payload);
    free(packet);
}

uint8_t* retrieve_flags(packet_t* packet){
    uint8_t* flags = malloc(8*sizeof(uint8_t));
    int idx = 0;
    for(int i = 128; i>=1; i=i/2){
        if(packet->flags >= i) {
            packet->flags -= i;
            flags[idx] = 1;
        }
        idx++;
    }
    return flags;
}



// ###########################" END UTIL ##################################"


// ONLY FOR TESTS PURPOSES
void print_flags(packet_t* packet){ 
    uint8_t* flags = retrieve_flags(packet);
    printf("[");
    for(int i = 0; i<7; i++){
        printf("%d, ", flags[i]);
    }
    printf("%d]\n", flags[7]);
    free(flags);
}

int main(){
    packet_t packet;
    packet.checksum = 0xff;
    packet.flags = TCP+SYN+ACK;
    packet.packet_number = 0;
    packet.version = VERSION;
    packet.src = "aaaaaaaaaaaaaaaaa";
    packet.dst = "bbbbbbbbbbbbbbbbb";
    packet.payload = "Hello UDP :)";
    char* buffer  = encode(&packet);

    packet_t* decoded = decode(buffer);
    
    printf("FLAGS: "); 
    print_flags(decoded);


    printf("SRC: %s\n",decoded->src);
    printf("DST: %s\n", decoded->dst);
    printf("Payload: %s\n", decoded->payload);

    free_packet(decoded);
    free(buffer);
    exit(0);
}
