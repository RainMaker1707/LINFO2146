#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include "lib/random.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (8 * CLOCK_SECOND)

/*---------------------------------------------------------------------------*/
PROCESS(gateway, "gateway process");
AUTOSTART_PROCESSES(&gateway);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gateway, ev, data)
{
  //static struct etimer periodic_timer;
  
  PROCESS_BEGIN();

  while(1) {
    PROCESS_YIELD();
    // receive message from the server
    if(ev==serial_line_event_message){
        // forward message to sub_gateway
        char* src = NULL; //TODO change
        char* dest = NULL; //TODO get address of sub_gateway
        packet_t* message_pkt = create_packet(UDP, 0, src, dest, (char*)data);
        unreliable_send(message_pkt, UNICAST, dest);
    }
    // receive message from sub_gateway
    else if(ev==nullnet_event_message){
        // forward message to the server
        char* src = NULL; //TODO change
        char* dest = NULL; //TODO get address of the server
        packet_t* message_pkt = create_packet(UDP, 0, src, dest, (char*)data);
        unreliable_send(message_pkt, UNICAST, dest);
    }
        
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/