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


#define SEND_INTERVAL (20 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
AUTOSTART_PROCESSES(&sender_process, &keep_alive, &discover_process);



void callback(packet_t* packet){
    LOG_INFO("UDP PACKET RECEIVED\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_node(TYPE_SENSOR+TYPE_LIGHT_BULB, SENSOR, callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("PARENT: ");
        if(get_parent() != NULL) LOG_INFO_LLADDR((linkaddr_t*)&(get_parent()->adress));
        printf("\n");
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

