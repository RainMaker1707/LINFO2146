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
#include "dev/button-sensor.h"


PROCESS(mobile_process, "mobile terminal process");
AUTOSTART_PROCESSES(&mobile_process, &keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("Received ack for maintenance message from ");
    LOG_INFO_LLADDR(packet->src);
    LOG_INFO("\n");
}


PROCESS_THREAD(mobile_process, ev, data)
{
    PROCESS_BEGIN();

    SENSORS_ACTIVATE(button_sensor);

    set_device_type(MOBILE);
    setup_node(SENSOR, callback);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(ev==sensors_event && data == &button_sensor);
        LOG_INFO("SEND MAINTENANCE\n");
        send_maintenance();
    }
    PROCESS_END();
}

