#include "receiver.h"




void switch_response(packet_t* packet, const linkaddr_t *src, const linkaddr_t *dest){
    switch(packet->flags){
        case UDP:
            get_callback()(packet);
            break;
        case DIS:
            LOG_INFO("RECEIVED DIS\n");
            discover(packet, src, dest);
            send_prt(packet->src);
            break;
        case DIO:
            LOG_INFO("RECEIVED DIO\n");
            alive(packet);
            break;
        case DIS+ACK:
            LOG_INFO("RECEIVED DIS+ACK\n");
            discover(packet, src, dest);
            if(get_parent_config())send_prt(packet->src);
            break;
        case PRT:
            LOG_INFO("RECEIVED PRT\n");
            receive_prt(packet, (linkaddr_t*)src);
            break;
        case PRT+ACK:
            LOG_INFO("RECEIVED PRT+ACK\n");
            attach_parent(packet, packet->src);
            break;
        case PRT+NACK:
            LOG_INFO("RECEIVED PRT+NACK\n");
            break;
        default:
            LOG_INFO("PACKET ERROR FLAGS\n");
    }
}

/*
*/
void receive(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest){
    packet_t* packet = decode((char*)data);
    if(packet == NULL) return;
    if(!linkaddr_cmp(packet->dst, &linkaddr_node_addr) && !linkaddr_cmp(packet->dst, &linkaddr_null)) return;
    switch_response(packet, src, dest);
    free_packet(packet);
}