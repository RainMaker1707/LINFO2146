#include "contiki.h"
#include "os/sys/clock.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "network_protocol/protocol/unreliable.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define Z_NUMBER 5
#define SEND_INTERVAL (8 * CLOCK_SECOND)

PROCESS(mobile_maintenance_process, "mobile maintenance process");
PROCESS(connection_process, "connection process");


AUTOSTART_PROCESSES(&mobile_maintenance_process, &connection_process);

/*------------------------------------*/
void maintenance_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(packet_t)) {
    //TODO log the received packet
  }
}
/*------------------------------------*/


/**
 * Process of the mobile terminal for maintenance.
 * The process will send Z_NUMBER packets to the light sensor
 * and wait for the response for each message sent.
 * 
 * */ 
PROCESS_THREAD(mobile_maintenance_process, ev, data)
{
    static struct etimer periodic_timer;
    static int packets_sent = 0;

    PROCESS_BEGIN();

    nullnet_set_input_callback(maintenance_callback);

    etimer_set(&periodic_timer, SEND_INTERVAL);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));


        char* src = NULL; //TODO change
        char* dest = NULL; //TODO get address of the light sensor
        packet_t* message_pkt = create_packet(UDP, 0, src, dest,"maintenance");

        //char* buffer = encode(message_pkt);
        unreliable_send(message_pkt, UNICAST, dest);
        
        packets_sent++;
        if(packets_sent == Z_NUMBER){
            break;
        }

        etimer_reset(&periodic_timer);
    }

    PROCESS_END();
}



/**
 * Process of the mobile terminal for the connection to a greenhouse.
*/
PROCESS_THREAD(connection_process, ev, data)
{
    PROCESS_BEGIN();

    //TODO setup connection to the sub network of the greenhouse

    PROCESS_END();
}