# Network Reliable Protocol

In the following TCP means reliable connection and UDP means unreliable.\
This protocol is not exactly UDP or TCP but it is inspired from they behavior.

## Goal

## V1
### Packet
Packets are composed of 256  bits. So 32 bytes.\
```

  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 
 +---------------+---------------------+-----------------------+-----------------------+
 |     Version   |        Flags        |      Source rank      |        Checksum       |
 +---------------+---------------------+-----------------------+-----------------------+
 |                                                                                     |
 |                                Source linkaddr                                      |
 +-------------------------------------------------------------------------------------+
 |                                                                                     |
 |                              Destination linkaddr                                   |
 +-------------------------------------------------------------------------------------+
 |                                                                                     |
 |                               Payload 96 bits                                       |
 |                                                                                     |
 +-------------------------------------------------------------------------------------+
 
 ```

#### Header
##### Version
    First byte store the version of the protocol used (for now version 1).

###### Flags
    Flags are stored on the second byte of the Header.\
    You can add the Flags you want to use at the same time, flags constant are stored in flags.h.\

 - **TCP** TCP: Mobile terminal maintenance messages
 - **MLT** Multicast: Used to send multicast messages.
 - **ACK** Acknoledged: #Packet set in was received.
 - **NACK** Not Acknoledged: #Packet set in was not received.
 - **RLY** Reset: abrupt interrupt the connection.
 - **DIO** Finish: gracefully terminates the connection.
 - **DIS** Discover: Packet sent to discover neighbour.
 - **PRT** Parent: Packet sent to ask to become the child to a parent
 - **UDP** Parent: Packet sent to ask to become the child to a parent

##### Source rank
    Third bytes of the Header, store the rank of the node that sends the packet.



