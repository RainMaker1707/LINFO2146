#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "utils/flags.h"
#include "protocol/net_process.h"


#define SEND_INTERVAL (35 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
AUTOSTART_PROCESSES(&sender_process,&keep_alive_process, &dis_process, &keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("NODE CALLBACK\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_custom_node(true, callback);
    check_neighbors_last_time_heard();
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        check_neighbors_last_time_heard();
        LOG_INFO("Inner process");
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}
