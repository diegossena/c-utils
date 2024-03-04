#pragma once

#include "sdk/application.h"
#include "sdk/net.h"
// error_code
#include "sdk/error.h"

/** @brief Represents a TCP server connection.
 *
 */
typedef struct net_tcp_client_t net_tcp_client_t;

typedef void (*net_tcp_client_on_write_cb)(net_tcp_client_t*, error_code, void* context);
typedef void (*net_tcp_client_on_read_cb)(net_tcp_client_t*, const byte* data, u64 length, void* context);

void net_tcp_client_close(net_tcp_client_t*);

void net_tcp_client_on_write(net_tcp_client_t*, net_tcp_client_on_write_cb);
void net_tcp_client_on_read(net_tcp_client_t*, net_tcp_client_on_read_cb);