#define TCP  128    // mobile terminal maintenance messages
#define MLT  64     // multicast from server
#define ACK  32     // ACKNOWLEDGED / OK
#define NACK 16     // NOT ACKNOWLEDGED / NOT OK
#define RLY  8      // RELAY broadcasted packet (RESERVED)
#define DIO  4      // ALIVE
#define DIS  2      // DISCOVER
#define PRT  1      // DECIDED PARENT
#define UDP 0       // if using UDP all flags to 0 (packet from mote/user)
