#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (8 * CLOCK_SECOND)

bool lightbulb_On = true;

void turn_Off() {
    lightbulb_On = false;
}

void turn_On() {
  lightbulb_On = true;
}


/*---------------------------------------------------------------------------*/
PROCESS(light_bulb_process, "Light Bulb Process");
AUTOSTART_PROCESSES(&light_bulb_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  
  unsigned message;
  memcpy(&message, data, sizeof(message));

  // 150 = TURN_OFF
  if (message == 150){
    turn_Off();
    LOG_INFO("Light Bulb: %s\n", lightbulb_On ? "ON" : "OFF");
  }
  
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(light_bulb_process, ev, data)
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


