#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct packet packet_t;

uint32_t ipv4_to_bits(char* ip);

uint8_t flags_to_bits(int* flags);

