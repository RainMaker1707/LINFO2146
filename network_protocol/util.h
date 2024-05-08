#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "flags.h" // Contain constant for flags. Add them to have multiple flags (not all combination works)

#define VERSION 0x01

typedef struct packet {
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


typedef reader* reader_pt;

uint32_t ipv4_to_bits(char* ip); // Not used

uint8_t flags_to_bits(int* flags);

packet_t create_packet(uint8_t flags, uint8_t packet_number, char* source_ip, char* dest_ip, char* payload);

bool check_packet(packet_t packet);

char* read_payload(packet_t packet);

reader_pt check_read(packet_t packet);


