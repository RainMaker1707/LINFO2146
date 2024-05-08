#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define VERSION 0x01

typedef struct packet packet_t;

uint32_t ipv4_to_bits(char* ip);

uint8_t flags_to_bits(int* flags);

packet_t create_packet(uint8_t flags, uint8_t packet_number, uint32_t source_ip, uint32_t dest_ip, uint32_t payload);