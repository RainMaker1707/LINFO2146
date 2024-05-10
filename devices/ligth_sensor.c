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
PROCESS(ligth_sensor, "light sensor process");
AUTOSTART_PROCESSES(&ligth_sensor);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ligth_sensor, ev, data)
{
  static struct etimer periodic_timer;
  
  static unsigned light_lvl;
  light_lvl = (unsigned) random_rand() % 101;
  PROCESS_BEGIN();

 etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    LOG_INFO("Sending light level %u to Sub_Gateway ", light_lvl);
    LOG_INFO_("\n");
    
    memcpy(nullnet_buf, &light_lvl, sizeof(light_lvl));
    nullnet_len = sizeof(light_lvl);

    NETSTACK_NETWORK.output(NULL);
    light_lvl = (unsigned) random_rand() % 101;
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
