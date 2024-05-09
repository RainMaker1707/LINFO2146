#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include <stdlib.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (1 * CLOCK_SECOND)


/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

int get_hex_val(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else {
    return -1;
  }
}

char* linkaddr_to_char(const linkaddr_t* addr) {
  if (addr == NULL) {
    return NULL;
  }

  char* str = malloc(16*sizeof(char));
  if (str == NULL) {
    return NULL;
  }

  for(int i = 0; i<LINKADDR_SIZE; i++){
    sprintf(str + 2*i, "%02x", addr->u8[i]);
  }

  return str; 
}

linkaddr_t* char_to_linkaddr(char* str) {
  if (str == NULL) {
    return NULL;
  }

  linkaddr_t* addr = (linkaddr_t*)malloc(sizeof(linkaddr_t));
  if (addr == NULL) {
    return NULL;
  }
  
  for (int i = 0; i < LINKADDR_SIZE; i++) {
    int char1 = get_hex_val(str[2*i]);
    int char2 = get_hex_val(str[2*i + 1]);
    unsigned char c = (char1 << 4) | char2;
    addr->u8[i] = c;  
  }

  return addr;
}

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)) {
    unsigned count;
    memcpy(&count, data, sizeof(count));
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");

    char* str = linkaddr_to_char(src); // Convert src address to string
    printf("src address to string is  %s\n", str); // Print the string

    linkaddr_t* str_address = char_to_linkaddr(str); // Convert the string back to linkaddr_t
    LOG_INFO("test2 is ");
    LOG_INFO_LLADDR(str_address); // Print the linkaddr_t (should be equal to src
    LOG_INFO_("\n");
    printf("cmp %d\n", linkaddr_cmp(str_address, src)); // Compare the two linkaddr_t (should be equal)
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
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
/*---------------------------------------------------------------------------*/