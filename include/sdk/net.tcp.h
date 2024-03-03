#pragma once

#include "sdk/application.h"
#include "sdk/stream.h"
#include "sdk/net.h"
#include "sdk/error.h"

/** @brief Represents a TCP stream or TCP server.
 *
 */
class net_tcp_t net_tcp_t;

typedef void (*net_tcp_on_connect_cb)(stream_t*);

/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp_t* net_tcp_new();
/** @brief function to create a net_tcp object
 * @param this A pointer which holds the created net_tcp object as provided.
 */
void net_tcp_free(net_tcp_t*);

void net_tcp_on_connect(net_tcp_t*, net_tcp_on_connect_cb);
/** @brief set socket ipv4 address to bind or connect
 * @param this net_tcp_t object
 * @param port port to listen or connect
 * @param host? host to connect
 */
error_code net_tcp_ip4_addr(net_tcp_t*, u16 port, const char* host);

error_code net_tcp_connect(net_tcp_t*);
error_code net_tcp_listen(net_tcp_t*);
