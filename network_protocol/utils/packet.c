#include "packet.h"

// ######################### API ########################################


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
packet_t* create_packet(uint8_t flags, uint8_t packet_number, char* source_ip, char* dest_ip, char* payload){
    packet_t* packet = malloc(sizeof(packet_t));
    packet->version = VERSION;
    packet->flags = flags;
    packet->packet_number = packet_number;
    packet->src = source_ip;
    packet->dst = dest_ip;
    packet->checksum = compute_checksum(packet); // TODO, for now 1byte at 1
    packet->payload = payload;
    return packet;
}

/*
    Function that check the checksum of the packet by computing its own checksum
    @Param: packet_t* packet: Pointer to the packet you want to check
    @Returns: true if checksum are equal, else false
*/

bool check_packet(packet_t* packet){
    if(packet->checksum == compute_checksum(packet)){
        return true;
    }
    return false;
}


/*
    Compute the checksum of the packet from all other field than its own checksum.
    @Returns: int of 1 bytes checksum
*/
uint8_t compute_checksum(packet_t* packet){
    // TODO
    return 0xff;
}

/*
    Check the packet checksum and return a reader object which contain the two answer 
    @Param: packet_t* packet: Pointeur to the packet you want to check and read
    @Returns: reader{
        bool check -> true if ok, false if packet corrupted
        char* payload -> if check == true, char* that contain the payload, else NULL
    }
*/
reader_t* check_read_packet(packet_t* packet) {
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


/*
    Encode the packet in a char buffer the packet to send
*/
char* encode(packet_t* packet){
    char* buffer = malloc(sizeof(char)*PACKET_SIZE);
    buffer[0] = VERSION;
    buffer[1] = packet->flags;
    buffer[2] = packet->packet_number;
    buffer[3] = packet->checksum;
    for(int i = 0; i<16; i++){
        buffer[4+i] = packet->src[i];
        if(packet->dst != NULL) buffer[20+i] = packet->dst[i];
    }
    for(int i = 0; i<12; i++){
        buffer[36+i] = packet->payload[i];
    }
    return buffer;
}


/*
    Decode the received buffer in a readable packet
*/
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


/*
    As the packet contains malloced char* you need to free it from this function
*/
void free_packet(packet_t* packet){
    free(packet->src);
    free(packet->dst);
    free(packet->payload);
    free(packet);
}


/*  
    Retrieve the list of flags from the raw uint8_t
    @Param: packet_t* packet: the decoded packet you want to read
    @Returns: uint8_t* to a uint8_t array of 8 elements
                [TCP, SYN, ACK, NACK, RST, FIN, DIS, PRT]
                [0, 0, 0, 0, 0, 0, 0, 0] == UDP
                [1, 1, 1, 0, 0 ,0 ,0 ,0] == TCP SYN ACK
*/
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


/*
    Computes the int value of a char representing a hexadecimal value
    @Param: char c: the char to convert
    @Returns: int value of the char in hexadecimal
*/
int get_hex_val(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else {
    return -1;
  }
}


/*
    Convert a linkaddr_t address to a char* address
    @Param: linkaddr_t* addr: the linkaddr_t to convert
    @Returns: char* string of the linkaddr_t
*/
char* linkaddr_to_char(const linkaddr_t* addr) {
  if (addr == NULL) {
    return NULL;
  }

  char* str = malloc(16*sizeof(char));
  if (str == NULL) {
    return NULL;
  }

  for(int i = 0; i<LINKADDR_SIZE; i++){
    sprintf(str + 2*i, "%02x", addr->u8[i]);
  }

  return str; 
}


/*
    Convert a char* address to a linkaddr_t address
    @Param: char* str: the string to convert
    @Returns: linkaddr_t* address of the string
*/
linkaddr_t* char_to_linkaddr(char* str) {
  if (str == NULL) {
    return NULL;
  }

  linkaddr_t* addr = (linkaddr_t*)malloc(sizeof(linkaddr_t));
  if (addr == NULL) {
    return NULL;
  }
  
  for (int i = 0; i < LINKADDR_SIZE; i++) {
    int char1 = get_hex_val(str[2*i]);
    int char2 = get_hex_val(str[2*i + 1]);
    unsigned char c = (char1 << 4) | char2;
    addr->u8[i] = c;  
  }

  return addr;
}