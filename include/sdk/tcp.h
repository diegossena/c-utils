#ifndef TCP_H
#define TCP_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef socket_t tcp_t;

export tcp_t tcp_new();
export void tcp_free(tcp_t);
export error_t tcp_connect(tcp_t this, ip4_t ip4, net_port_t port, u64 timeout_ms);
export i32 tcp_read(tcp_t this, char* target, u64 size);
export i32 tcp_send(tcp_t this, const char* buffer, u64 size);

#endif