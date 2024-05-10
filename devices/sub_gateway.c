#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */

#define LIGHT_THRESHOLD 20

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (8 * CLOCK_SECOND)


PROCESS(sub_gateway_process, "Sub-Gateway Process");
AUTOSTART_PROCESSES(&sub_gateway_process);

void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)) {
    unsigned light_level;
    // Receive light level from the sensor
    memcpy(&light_level, data, sizeof(light_level));
    LOG_INFO("Received light sensor Info: %u from ", light_level);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
    
    // Check if light level is below threshold
    if (light_level < LIGHT_THRESHOLD) {
      // Send command to turn on the light bulb
      unsigned message = 150;
      
      LOG_INFO("Sending TURN_ON to lightbulb ");
      LOG_INFO_("\n");
      memcpy(nullnet_buf, &message, sizeof(message));
      nullnet_len = sizeof(message);
    }
  }
}

PROCESS_THREAD(sub_gateway_process, ev, data)
{
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  nullnet_set_input_callback(input_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    NETSTACK_NETWORK.output(NULL);
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
