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


#define SEND_INTERVAL (10 * CLOCK_SECOND)

PROCESS(sender_process, "light sensor");
AUTOSTART_PROCESSES(&sender_process, &keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("Received a maintenance message\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    set_device_type(LIGHT_SENSOR);
    setup_node(SENSOR, callback);
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_light();
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

