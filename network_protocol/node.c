#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "protocol/unreliable.h"


#define SEND_INTERVAL (80 * CLOCK_SECOND)


PROCESS(sender_process, "Node example for DIS Tree creation");
AUTOSTART_PROCESSES(&sender_process);



void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  LOG_INFO("RECEIVED PACKET of len %d!\n", len);
  char encoded[PACKET_SIZE];
  memcpy(&encoded, data, PACKET_SIZE);

  packet_t* packet = decode((char*)&encoded);
  if(packet == NULL) {
    LOG_INFO("Error decoding\n");
    return;
  }
  LOG_INFO("Received %s from ", packet->payload);
  LOG_INFO_LLADDR(src);
  LOG_INFO_("\n");
}


PROCESS_THREAD(sender_process, ev, data)
{
    static struct etimer periodic_timer;
    PROCESS_BEGIN();

    nullnet_set_input_callback(input_callback);
    
    setup_node();

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        packet_t *packet = create_packet(DIS, 0, "aaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaa", "Hello net!");
        unreliable_send(packet, BROADCAST);
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

