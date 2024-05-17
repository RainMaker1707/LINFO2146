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

#include "utils/flags.h"
#include "protocol/net_process.h"


#define SEND_INTERVAL (30 * CLOCK_SECOND)

PROCESS(irrigation_system, "irrigation system");
AUTOSTART_PROCESSES(&irrigation_system, &keep_alive_process);



void callback(packet_t* packet){

    LOG_INFO("Command start irrigation for %s minute(s) \n", packet->payload);
}


PROCESS_THREAD(irrigation_system, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    set_device_type(IRRIGATION);
    setup_node(SENSOR, callback);
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}



