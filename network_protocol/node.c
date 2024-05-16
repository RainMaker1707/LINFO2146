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

#include "net_api/setup.h"

#include "constants/constant.h"
#include "constants/types.h"


#define SEND_INTERVAL (60 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
AUTOSTART_PROCESSES(&sender_process, &keep_alive, &discover_process);



void callback(packet_t* packet){
    LOG_INFO("PACKET RECEIVED\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_node(TYPE_BULB, SENSOR, callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("ALL OK\n");
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

