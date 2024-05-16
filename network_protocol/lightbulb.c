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


#define SEND_INTERVAL (30 * CLOCK_SECOND)

bool lightbulb_On = false;

void turn_off() {
  lightbulb_On = false;
}

void turn_on() {
  lightbulb_On = true;
}

PROCESS(light_bulb, "Light Bulb");
AUTOSTART_PROCESSES(&light_bulb, &keep_alive_process);



void callback(packet_t* packet){
    LOG_INFO("light CALLBACK \n");
}

void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  
  unsigned message;
  memcpy(&message, data, sizeof(message));

  // 150 = TURN_ON
  if (message == 150 && !lightbulb_On){
    //turn on
    turn_on();
    LOG_INFO("Light Bulb: %s\n", lightbulb_On ? "ON" : "OFF");
    
    // turn off after a certain time
    clock_wait(SEND_INTERVAL*2);
    
    turn_off();
    LOG_INFO("Light Bulb: %s\n", lightbulb_On ? "ON" : "OFF");
  }
  
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
        print_table();
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}



