
/**
 * \file
 *         Light sensor 
 * \author
 *         Cyril Lapierre
 */

#include "contiki.h"
#include "cpu/msp430/dev/uart0.h"

#include <stdlib.h>
#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(ligth_sensor, "light sensor process");
AUTOSTART_PROCESSES(&ligth_sensor);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ligth_sensor, ev, data)
{

  PROCESS_BEGIN();

  printf("Starting process\n");
  while(1) {
	
    // send every 10 secondes sensor info
    PROCESS_YIELD();
    double light_level = (double)rand()/(double)RAND_MAX ;
    
    process_post(&sub_gateway_process, light_event, &light_level);

    sleep(10)
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
