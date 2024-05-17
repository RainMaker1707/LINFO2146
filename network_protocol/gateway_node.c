#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "utils/flags.h"
#include "protocol/net_process.h"
#include "dev/serial-line.h"
#include "cpu/msp430/dev/uart0.h"


#define SEND_INTERVAL (30 * CLOCK_SECOND)

PROCESS(sender_process, "Node example alive");
PROCESS(test_serial, "Test serial");
AUTOSTART_PROCESSES(&sender_process, &keep_alive_process, &test_serial);


void callback(packet_t* packet){
    LOG_INFO("## GATEWAY NODE CALLBACK\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    
    set_device_type(GATEWAY);
    setup_gateway(GATEWAY, true, false, callback);
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("GATEWAY Inner process \n");
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

PROCESS_THREAD(test_serial, ev, serv_data)
{

    //static struct etimer timer;

    PROCESS_BEGIN();
    /* start tools to communicate with the server*/
    serial_line_init();
    uart0_set_input(serial_line_input_byte);
    

    while(1) {
        
        PROCESS_YIELD();
        if(ev==serial_line_event_message){
            handle_server_response(serv_data);

        }
    }
  PROCESS_END();
}