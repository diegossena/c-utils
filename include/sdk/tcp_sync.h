#ifndef TCP_SYNC_H
#define TCP_SYNC_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef u64 tcp_socket_t;

export tcp_socket_t tcp_sync_new();
export error_t tcp_sync_connect(tcp_socket_t this, ip4_t ip4, net_port_t port, u64 timeout_ms);
export void tcp_sync_free(tcp_socket_t);

#endif