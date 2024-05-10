#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"
#include "utils/flags.h"
#include "protocol/unreliable.h"


#define DIS_INTERVAL (60 * CLOCK_SECOND)
#define ALIVE_INTERVAL (10 * CLOCK_SECOND)

PROCESS(keep_alive_process, "alive process");
PROCESS(dis_process, "process discovery neighbour");


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


PROCESS_THREAD(dis_process, ev, data){
    static struct etimer period;
    PROCESS_BEGIN();
    etimer_set(&period, DIS_INTERVAL);
    while(1){
        packet_t *packet = create_packet(DIS, 0, &linkaddr_node_addr, NULL, "alive");
        unreliable_send(packet, BROADCAST);
        free_packet(packet);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&period));
    }
    PROCESS_END();
}
