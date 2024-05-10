#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "protocol/unreliable.h"


#define SEND_INTERVAL (80 * CLOCK_SECOND)


PROCESS(sender_process, "Node example for DIS Tree creation");
AUTOSTART_PROCESSES(&sender_process);



void callback(packet_t* packet){
    LOG_INFO("NODE CALLBACK\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();

    setup_node(callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        //packet_t *packet = create_packet(UDP, 0, linkaddr_node_addr, "aaaaaaaaaaaaaaaa", "Hello net!");
        //unreliable_send(packet, BROADCAST);
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

