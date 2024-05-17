#pragma once

#include "constants/constant.h"
#include "constants/flags.h"
#include "net_api/setup.h"
#include "alive.h"



/*
    Send a packet with PRT flag to ask a node to be parent
*/
void send_prt(linkaddr_t* dest);
void send_prt_ack(linkaddr_t* dest);
void send_prt_nack(linkaddr_t* dest);



/*
    Function that define the behavior when your receive a PRT packet
*/
void receive_prt(packet_t* packet, linkaddr_t* src);



/*
*/
void attach_parent(packet_t* packet, linkaddr_t* addr);



/*
    return X an int to indicate which mote has the higher signal strength
        if X == 0: signal are the same
        if X > 0: mote1 has the highest signal
        if X < 0: mote2 has the highest signal
*/
int compare_signal_strength(mote_t* mote1, mote_t* mote2);