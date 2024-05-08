#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>




#define VERSION 0x01


typedef struct packet {
    uint8_t packet_number; // just for reliability (like TCP)
    uint8_t flags; // 8 bits (as TCP) if one bit is set it is a certain flag for example 10000000 == SYN packet, 01000000 == ACK packet ...
    uint8_t checksum;
    char src[16]; // Source IPv6 128 bits -> 16 bytes
    char dst[16]; // Destination IPv6 128 bits -> bytes
    char payload[12]; // Payload converted in 96 bits -> 12 bytes
} packet_t;


typedef struct reader{
    bool checker;
    char* payload;
} reader_t;


typedef reader* reader_pt;

uint32_t ipv4_to_bits(char* ip); // TODO: change to IPV6

uint8_t flags_to_bits(int* flags);

packet_t create_packet(uint8_t flags, uint8_t packet_number, char* source_ip, char* dest_ip, char* payload);

bool check_packet(packet_t packet);

char* read_payload(packet_t packet);

reader_pt check_read(packet_t packet);


