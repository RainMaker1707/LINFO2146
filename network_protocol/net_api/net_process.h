#pragma once

#include "receiver.h"
#include "sender.h"
#include "setup.h"

#define DISCOVER_INTERVAL (120 * CLOCK_SECOND)
#define ALIVE_INTERVAL (20 * CLOCK_SECOND)


PROCESS(discover_process, "discover process");
PROCESS(keep_alive, "alive process");


PROCESS_THREAD(discover_process, ev, data){
    static struct etimer period;
    PROCESS_BEGIN();
    etimer_set(&period, DISCOVER_INTERVAL);
    while(1){
        send_discover();
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&period));
        etimer_reset(&period);
    }
    PROCESS_END();
}


PROCESS_THREAD(keep_alive, ev, data){
    static struct etimer periodic_timer;
    PROCESS_BEGIN();
    etimer_set(&periodic_timer, ALIVE_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        heartbeat(get_rank());
        check_neighbors(get_neighbors());
        etimer_reset(&periodic_timer);
    }
    PROCESS_END();
}




