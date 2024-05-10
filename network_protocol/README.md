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
 |     Version   |        Flags        |        #Packet        |        Checksum       |
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

 - **PTC** Protocol: 0 if UDP, 1 if TCP. (TCP for V3)
 - **SYN** Synchronization: Used to engage a connection.
 - **ACK** Acknoledged: #Packet set in was received.
 - **NACK** Not Acknoledged: #Packet set in was not received.
 - **RST** Reset: abrupt interrupt the connection.
 - **FIN** Finish: gracefully terminates the connection.
 - **DIS** Discover: Packet sent to discover neighbour.
 - **PRT** Parent: Packet sent to ask to become the child to a parent

##### Packet number
    Third bytes of the Header, store the packet number for reliable connection.

##### Checksum
    Fourth byte, need to find an explanation by it fill the byte gap 

### Tree formation
### Send packet
### Receive packet

### Unicast

### Broadcast
Packet X is received with dst adress set to the broadcast ipv6 adress.\
Two possibilities:\
 - Node has child and then read message, take action if needed and finally forward the message to all its childs
 - Node has not child, it will read the message and then discard it after taking action if needed



## V2
### Goal
Add MultiCast on top of the V1

### Packet update
TODO

```

  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 
 +---------------+---------------------+-----------------------+-----------------------+
 |     Version   |        Flags        |        #Packet        |        Checksum       |
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

### Multicast


## V3
### Goal
Add TCP over this V2 protocol (flags are ready so it is feasible with enough time)