# Network Reliable Protocol

## Goal

## V1
### Packet
Packets are composed of 384  bits. So 48 bytes.\
```

  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 
 +---------------+---------------------+-----------------------+-----------------------+
 |     Version   |        Flags        |        #Packet        |        Checksum       |
 +---------------+---------------------+-----------------------+-----------------------+
 |                                                                                     |
 |                                Source IPv6                                          |
 |                                                                                     |
 |                                                                                     |
 +-------------------------------------------------------------------------------------+
 |                                                                                     |
 |                              Destination IPv6                                       |
 |                                                                                     |
 |                                                                                     |
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

 - **PTC** Protocol: 0 if UDP, 1 if TCP. (TCP for V3)
 - **SYN** Synchronization: Used to engage a connection.
 - **ACK** Acknoledged: #Packet set in was received.
 - **NACK** Not Acknoledged: #Packet set in was not received.
 - **RST** Reset: abrupt interrupt the connection.
 - **FIN** Finish: gracefully terminates the connection.
 - **DIS** Discover: Packet sent to discover neighbour.
 - **PRT** Parent: Packet sent to ask to become the child to a parent

##### Packet number
    Third bytes of the Header, store the packet number for reliable connection
##### Checksum
    Fourth byte, need to find an explanation by it fill the byte gap 


### Encoder
### Decoder
### Tree formation
### Send packet
### Receive packet

### Unicast
### Broadcast



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
 |                                Source IPv6                                          |
 |                                                                                     |
 |                                                                                     |
 +-------------------------------------------------------------------------------------+
 |                                                                                     |
 |                              Destination IPv6                                       |
 |                                                                                     |
 |                                                                                     |
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