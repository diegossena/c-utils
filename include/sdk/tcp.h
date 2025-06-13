#ifndef TCP_H
#define TCP_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef socket_t tcp_t;

tcp_t tcp_new();
void tcp_free(tcp_t);
error_t tcp_connect(tcp_t this, ip4_t ip4, net_port_t port, u64 timeout_ms);
error_t tcp_read(tcp_t this, char* target, u64 size);
error_t tcp_send(tcp_t this, const char* buffer, u64 size);

#endif