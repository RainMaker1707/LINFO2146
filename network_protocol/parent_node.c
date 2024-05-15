#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "utils/flags.h"
#include "protocol/net_process.h"


#define SEND_INTERVAL (30 * CLOCK_SECOND)

# define RANK 1

PROCESS(sender_process, "Node example alive");
AUTOSTART_PROCESSES(&sender_process, &keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("##  PARENT NODE CALLBACK\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    set_device_type(SUBGATEWAY);
    setup_subgateway(SUBGATEWAY, true, callback);
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        // LOG_INFO("SG PARENT: ");
        // if(parent!=NULL) LOG_INFO_LLADDR((linkaddr_t*)&(parent->adress));
        // LOG_INFO("\n");
        print_table();
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}
