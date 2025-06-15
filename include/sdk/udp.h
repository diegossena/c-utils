#ifndef UDP_H
#define UDP_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef socket_t udp_t;

udp_t udp_new();
void udp_free(udp_t);

error_t udp_bind(udp_t this, net_port_t port);
error_t udp_read(udp_t this, u8* target, u64 size, net_address_t* address);
error_t udp_send(udp_t this, const u8* buffer, u64 length, net_address_t* address);

#endif