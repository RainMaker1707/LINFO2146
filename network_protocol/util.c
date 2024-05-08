#include "util.h"


struct packet {
    uint32_t src; // Source IPv4 32 bits
    uint32_t dst; // Destination IPv4 32 bits
    uint8_t packet_number; // just for reliability (like TCP)
    uint8_t flag; // 8 bits (as TCP) if one bit is set it is a certain flag for example 10000000 == SYN packet, 01000000 == ACK packet ...
    uint16_t checksum;
    uint32_t payload; // Payload converted in 64 bits
};


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
    @Param: int* flags: int array, len 8, if flags[i] <= 0 then bit[i] == 0, else bit[i] == 1 and then flag is set 
    @Return: uint8_t where bit[i] is a flag described in the README.md of this directory 
*/
uint8_t flags_to_bits(int* flags){
    
}


void main(){
    char ip[16] = "192.255.255.255";
    ipv4_to_bits(&(ip[0]));
}
