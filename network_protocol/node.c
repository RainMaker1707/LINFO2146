#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"


#include "net_api/net_process.h"

#include "constants/constant.h"


#define SEND_INTERVAL (31 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
AUTOSTART_PROCESSES(&sender_process);



void callback(packet_t* packet){
    LOG_INFO("PACKET RECEIVED\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_node(SENSOR, callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("PREPARE PACKET\n");
        packet_t* packet = create_packet(TYPE_BULB, UDP, SENSOR, &linkaddr_node_addr, &linkaddr_null, "DISCOVER");
        send(packet, BROADCAST);
        LOG_INFO("PACKET SENT\n");
        free_packet(packet);
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

