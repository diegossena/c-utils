#pragma once

#include "sdk/net.tcp.client.h"
#include "sdk/error.h"

/** @brief Represents a TCP stream or TCP server.
 *
 */
typedef struct net_tcp_server_t net_tcp_server_t;

typedef void (*net_tcp_on_connection_cb)(net_tcp_client_t*);

/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp_server_t* net_tcp_server_new();


error_code net_tcp_server_ip4_listen(net_tcp_server_t*, u16 port, net_tcp_on_connection_cb);
