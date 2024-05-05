#include "contiki.h"
#include "sub_gateway.h" // Include header file for shared definitions

#define LIGHT_THRESHOLD 0.75

PROCESS(sub_gateway_process, "Sub-Gateway Process");
AUTOSTART_PROCESSES(&sub_gateway_process);

PROCESS_THREAD(sub_gateway_process, ev, data)
{
  PROCESS_BEGIN();

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == light_event);

    // Receive light level from the sensor
    int light_level = *((int *)data);

    // Check if light level is below threshold
    if (light_level < LIGHT_THRESHOLD) {
      // Send command to turn on the light bulb
      process_post(&light_bulb_process, control_event, NULL);
    }
  }

  PROCESS_END();
}
