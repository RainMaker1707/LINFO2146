# Network Reliable Protocol

## Goal

## Packet
Packets are composed of 128 bits.\
```
 
 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 
 +--------------+---------------------+-----------------------+-----------------------+
 |    Version   |        Flags        |        #Packet        |        Checksum       |
 +--------------+---------------------+-----------------------+-----------------------+
 |                               Source IPv4                                          |
 +------------------------------------------------------------------------------------+
 |                             Destination IPv4                                       |
 +------------------------------------------------------------------------------------+
 |                                 PAYLOAD                                            |
 +------------------------------------------------------------------------------------+
 ```


### Flags
 - **SYN** Synchronization: Used to engage a connection.
 - **ACK** Acknoledged: #Packet set in was received.
 - **NACK** Not Acknoledged: #Packet set in was not received.
 - **CNT** Continue: if the bit is set it means you have to wiat for others packets before using it.
 - **TR3** Tree: packet used to create the tree.
 - **RST** Reset: abrupt interrupt the connection.
 - **FIN** Finish: gracefully terminates the connection.
 - **TEA** Packet is a tea pot.


## Encoder
## Decoder
## Tree formation
## Send packet
## Receive packet