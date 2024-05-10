#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "utils/flags.h"
#include "protocol/unreliable.h"


#define SEND_INTERVAL (35 * CLOCK_SECOND)
#define ALIVE_INTERVAL (10 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
PROCESS(keep_alive_process, "alive process");
AUTOSTART_PROCESSES(&sender_process,&keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("NODE CALLBACK\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_node(callback);
    check_neighbors_last_time_heard();
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        check_neighbors_last_time_heard();
        //packet_t *packet = create_packet(UDP, 0, linkaddr_node_addr, "aaaaaaaaaaaaaaaa", "Hello net!");
        //unreliable_send(packet, BROADCAST);
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}


PROCESS_THREAD(keep_alive_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    etimer_set(&periodic_timer, ALIVE_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        
        packet_t *packet = create_packet(FIN, 0, &linkaddr_node_addr, NULL, "alive");
        unreliable_send(packet, BROADCAST);

        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}



