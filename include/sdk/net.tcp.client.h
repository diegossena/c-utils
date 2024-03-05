#pragma once

#include "sdk/application.h"
#include "sdk/net.h"
// error_code
#include "sdk/error.h"

/** @brief Represents a TCP server connection.
 *
 */
typedef struct net_tcp_client_t net_tcp_client_t;

typedef void (*net_tcp_client_on_write_cb)(net_tcp_client_t* this, const void* context);
typedef void (*net_tcp_client_on_read_cb)(net_tcp_client_t* this, const byte* data, u64 length, const void* context);

void net_tcp_client_write(net_tcp_client_t* this, const byte* chunk, u64 length, net_tcp_client_on_write_cb callback);
void net_tcp_client_read(net_tcp_client_t* this, u64 length, net_tcp_client_on_read_cb callback);