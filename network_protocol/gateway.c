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

void print_table(){
    if(get_neighbors()->head == NULL) {
        LOG_INFO("## NO NEIGHBORS\n");
        return;
    }
    int counter = 0;
    node_t* current = get_neighbors()->head;
    while(1){
        LOG_INFO("%d: ADD: ", counter);
        LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->adress));
        LOG_INFO("\n");
        LOG_INFO("%d: SRC: ", counter++);
        LOG_INFO_LLADDR((linkaddr_t*)&(current->mote->src));
        LOG_INFO("\n");
        if(current == get_neighbors()->tail) return;
        current = current->next;
    }
}


void callback(packet_t* packet){
    LOG_INFO("UDP PACKET RECEIVED\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    
    setup_node(TYPE_GATEWAY, GATEWAY, callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("GATEWAY\n");
        print_table();
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

