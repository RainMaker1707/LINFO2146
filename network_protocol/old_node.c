#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

/* Log configuration */
#include "contiki.h"
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include "sys/log.h"

#include "utils/packet.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SEND_INTERVAL (8 * CLOCK_SECOND)

static uint8_t buffer[PACKET_SIZE];

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


    etimer_set(&periodic_timer, SEND_INTERVAL);

    nullnet_buf = buffer;
    nullnet_len = PACKET_SIZE;

   while(1) {
        packet_t *packet = create_packet(DIS, 0, "aaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaa", "Hello net!");
        LOG_INFO("PACKET PAYLOAD: %s\n", packet->payload);
        uint8_t *encoded_data = (uint8_t*)encode(packet);
        memcpy(nullnet_buf, encoded_data, PACKET_SIZE);  // Use nullnet_buf directly
        LOG_INFO("BUFFER PAYLOAD: %s\n", decode((char*)encoded_data)->payload);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        LOG_INFO("Broadcast packet %s\n", decode((char*)nullnet_buf)->payload); // Verify using nullnet_buf
        NETSTACK_NETWORK.output(NULL);
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}

