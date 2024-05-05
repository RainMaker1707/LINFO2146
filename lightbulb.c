#include "contiki.h"

bool lightbulb_On = true;

void turn_Off() {
    lightbulb_On = false;
}

void turn_On() {
  lightbulb_On = true;
}


PROCESS(light_bulb_process, "Light Bulb Process");
AUTOSTART_PROCESSES(&light_bulb_process);

PROCESS_THREAD(light_bulb_process, ev, data)
{
  PROCESS_BEGIN();

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL();

    if(ev == turn_off_light){
        turn_Off();
    }
    else if (ev == turn_on_light){
        turn_On();
    }

    // Print the light bulb status
    printf("Light Bulb: %s\n", lightbulb_On ? "ON" : "OFF");
  }

  PROCESS_END();
}
