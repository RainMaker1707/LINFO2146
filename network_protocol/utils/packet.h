#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "flags.h" // Contain constant for flags. Add them to have multiple flags (not all combination works)

#define VERSION 0x01

typedef struct packet {
    uint8_t version;
    uint8_t packet_number; // just for reliability (like TCP)
    uint8_t flags; // 8 bits (as TCP) if one bit is set it is a certain flag for example 10000000 == SYN packet, 01000000 == ACK packet ...
    uint8_t checksum;
    char *src; // Source IPv6 128 bits -> 16 bytes
    char *dst; // Destination IPv6 128 bits -> bytes
    char *payload; // Payload converted in 96 bits -> 12 bytes
} packet_t;


typedef struct reader{
    bool checker;
    char* payload;
} reader_t;


/*
    Create a packet and compute the checksum.
    This function has to return a packet correctly formatted OR reject it.
    The checksum will ensure the packet has not been corrupted while the transportation.
    @Params: flags int array, len 8, if flags[i]<0 throw error, else if flags[i] == 0 then bit[i] == 0, else bit[i] == 1 and then flag is set 
                        [TCP, SYN, ACK, NACK, RST, FIN, DIS, PRT]
    @Param: packet_number
    @Param: source_ip
    @Param: dest_ip
    @Param: payload
    @Returns: packet_t containing all the information OR reject the packet and return NULL
*/
packet_t* create_packet(uint8_t flags, uint8_t packet_number, char* source_ip, char* dest_ip, char* payload);

/*
    Function that check the checksum of the packet by computing its own checksum
    @Param: packet_t* packet: Pointer to the packet you want to check
    @Returns: true if checksum are equal, else false
*/
bool check_packet(packet_t* packet);

/*
    Compute the checksum of the packet from all other field than its own checksum.
    @Returns: int of 1 bytes checksum
*/
uint8_t compute_checksum(packet_t* packet); 

/*
    Check the packet checksum and return a reader object which contain the two answer 
    @Param: packet_t* packet: Pointeur to the packet you want to check and read
    @Returns: reader{
        bool check -> true if ok, false if packet corrupted
        char* payload -> if check == true, char* that contain the payload, else NULL
    }
*/
reader_t* check_read_packet(packet_t* packet);

/*
    Encode the packet in a char buffer the packet to send
*/
char* encode(packet_t* packet);

/*
    Decode the received buffer in a readable packet
*/
packet_t* decode(char* packet);

/*
    As the packet contains malloced char* you need to free it from this function
*/
void free_packet(packet_t* packet);

/*  
    Retrieve the list of flags from the raw uint8_t
    @Param: packet_t* packet: the decoded packet you want to read
    @Returns: uint8_t* to a uint8_t array of 8 elements
                [TCP, SYN, ACK, NACK, RST, FIN, DIS, PRT]
                [0, 0, 0, 0, 0, 0, 0, 0] == UDP
                [1, 1, 1, 0, 0 ,0 ,0 ,0] == TCP SYN ACK
*/
uint8_t* retrieve_flags(packet_t* packet);