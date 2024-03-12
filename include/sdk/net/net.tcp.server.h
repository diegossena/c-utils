#pragma once
#include <sdk/error/error.h>

typedef struct net_tcp_client_t net_tcp_client_t;
typedef void (*net_tcp_on_connection_cb)(net_tcp_client_t*);

/** @brief Represents a TCP stream or TCP server.
 *
 */
typedef struct net_tcp_server_t net_tcp_server_t;
/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp_server_t* net_tcp_server_new(application_t*);
void net_tcp_server_free(net_tcp_server_t*);
// methods
error_code net_tcp_server_ip4_listen(net_tcp_server_t*, u16 port, net_tcp_on_connection_cb);
// application handlers
void net_tcp_server_close_handle(net_tcp_server_t*);
void net_tcp_server_listen_handle(net_tcp_server_t*);

#include <sdk/net/net.tcp.server.inl.h>
#include <sdk/net/tcp.server.win32.inl.h>