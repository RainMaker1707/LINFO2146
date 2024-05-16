#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#include "net/netstack.h" // import for linkaddr_t

#include "../constants/types.h"
#include "../constants/flags.h" // Contain constant for flags. Add them to have multiple flags (not all combination works)



#define PACKET_SIZE 32
#define ADDR_LEN 8
#define HEADER_LEN 4                // header is 4 bytes
#define PACKET_PAYLOAD_MAX_LEN 12   // Payload is 12 bytes max







// ################## PACKET struct definition ######################

typedef struct packet {
    uint8_t type;
    uint8_t src_rank; // just for reliability (like TCP)
    uint8_t flags; // 8 bits (as TCP) if one bit is set it is a certain flag for example 10000000 == SYN packet, 01000000 == ACK packet ...
    uint8_t checksum;
    linkaddr_t *src; // Source IP 8 bytes
    linkaddr_t *dst; // Destination IP 8 bytes
    char *payload; // Payload converted in 96 bits -> 12 bytes
} packet_t;


typedef struct reader{
    bool checker;
    char* payload;
} reader_t;


// ################### Functions definition ##########################


/*
    Create a packet and compute the checksum.
    This function has to return a packet correctly formatted OR reject it.
    The checksum will ensure the packet has not been corrupted while the transportation.
    @Params: flags int array, len 8, if flags[i]<0 throw error, else if flags[i] == 0 then bit[i] == 0, else bit[i] == 1 and then flag is set 
                        [TCP, SYN, ACK, NACK, RST, FIN, DIS, PRT]
    @Param: src_rank
    @Param: source_ip
    @Param: dest_ip
    @Param: payload
    @Returns: packet_t containing all the information OR reject the packet and return NULL
*/
packet_t* create_packet(uint8_t type, uint8_t flags, uint8_t src_rank, const linkaddr_t* source_ip, const linkaddr_t* dest_ip, char* payload);


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
packet_t* decode(char* buffer);


/*
    As the packet contains malloced memory you need to free it from this function
*/
void free_packet(packet_t* packet);


/*  
    Retrieve the list of flags from the raw uint8_t
    @Param: packet_t* packet: the decoded packet you want to read
    @Returns: uint8_t* to a uint8_t array of 8 elements
                [TCP, MLT, ACK, NACK, RLY, DIO, DIS, PRT]
                [0, 0, 0, 0, 0, 0, 0, 0] == UDP
                [1, 1, 1, 0, 0 ,0 ,0 ,0] == TCP MLT ACK
*/
uint8_t* retrieve_flags(packet_t* packet);





















// Maybe remove these

/*
    Computes the int value of a char representing a hexadecimal value
    @Param: char c: the char to convert
    @Returns: int value of the char in hexadecimal
*/
int get_hex_val(char c);


/*
    Convert a linkaddr_t address to a char* address
    @Param: linkaddr_t* addr: the linkaddr_t to convert
    @Returns: char* string of the linkaddr_t
*/
char* linkaddr_to_char(const linkaddr_t* addr);


/*
    Convert a char* address to a linkaddr_t address
    @Param: char* str: the string to convert
    @Returns: linkaddr_t* address of the string
*/
linkaddr_t* char_to_linkaddr(char* str);
