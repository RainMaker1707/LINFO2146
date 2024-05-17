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

PROCESS(light_bulb, "Light Bulb");
AUTOSTART_PROCESSES(&light_bulb, &keep_alive_process);



void callback(packet_t* packet){

    LOG_INFO("Received light on command. Turning on for %s minute(s).\n", packet->payload);
}


PROCESS_THREAD(light_bulb, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    set_device_type(LIGHT_BULB);
    setup_node(SENSOR, callback);
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}



