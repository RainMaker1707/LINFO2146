#include "util.h"


/*
    Convert a ipv4 in string to a 32bits uint to add in the packet
    @Params: char* ip: String of 15 char at most, formatted as an ipv4 human readable
    @Return: uint32_t which represent the ipv4
*/
uint32_t ipv4_to_bits(char* ip){
    uint32_t buffer = 0;
    uint8_t ip_part = 0;
    char ip_part_char[3];
    int i = 0;
    while(ip[i] != '\0'){
        if(ip[i] == '.' || ip[i+1] == '\0'){
            uint8_t part = atoi(ip_part_char);
            buffer = buffer << 8;
            buffer |= part;
        }else{
            ip_part_char[i%4] = ip[i];
        }
        i++;
    }
    return buffer;
}


/*
    Convert an int array to an 8 bits input to add in the packet
    @Param: int* flags: int array, len 8, if flags[i]<0 throw error, else if flags[i] == 0 then bit[i] == 0, else bit[i] == 1 and then flag is set 
                        [PTC, SYN, ACK, NACK, RST, FIN, DIS, PRT]
    @Return: uint8_t where bit[i] is a flag described in the README.md of this directory 
*/
uint8_t flags_to_bits(int* flags){
    uint8_t buffer = 0;
    for(int i = 0; i<8; i++){
        if(flags[i] < 0){
            printf("\033[1;31mError flags[%d]<0 not allowed\033[0m\n", i);
            exit(1);
        }
        buffer = buffer << 1;
        if(flags[i] > 0) { buffer |= 1;}
    }
    return buffer;
}


/*
    Create a packet and compute the checksum.
    This function has to return a packet correctly formatted OR reject it.
    The checksum will ensure the packet has not been corrupted while the transportation.
    @Params: flags int array, len 8, if flags[i]<0 throw error, else if flags[i] == 0 then bit[i] == 0, else bit[i] == 1 and then flag is set 
                        [PTC, SYN, ACK, NACK, RST, FIN, DIS, PRT]
    @Params: packet_number
    @Params: source_ip
    @Params: dest_ip
    @Params: payload
    @Return: packet_t containing all the information OR reject the packet and return NULL
*/
packet_t* create_packet(uint8_t flags, uint8_t packet_number, uint32_t source_ip, uint32_t dest_ip, uint32_t payload){
    packet_t* packet = malloc(sizeof(packet_t));
    packet->flags = flags;
    return packet;
}


reader_pt check_read(packet_t packet) {
    reader_t* toRet = (reader_t*) malloc(sizeof(reader_t));
    if(check_packet(packet)){
        toRet->checker = true;
        toRet->payload = read_payload(packet);
    } else {
        toRet->checker = false;
        toRet->payload = NULL;
    }
    return toRet;
}



// ONLY FOR TEST PURPOSES
int main(){
    int flags[8];
    for(int i = 0; i<8; i++){flags[i]=0;}
    flags[2] = 1;
    uint8_t final = flags_to_bits(flags);
    printf("Final flag uint8_t: %u\n", final);
    return 0;    
}
