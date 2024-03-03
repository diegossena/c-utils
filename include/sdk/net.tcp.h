#pragma once

#include "internal/application.h"
#include "internal/stream.h"
#include "internal/net.h"
#include "internal/handle.h"
#include "sdk/error.h"

// Represents a TCP stream or TCP server.
typedef struct net_tcp {
  stream tcp_stream;
  u64 socket;
} net_tcp;

net_tcp* net_tcp_new();
void net_tcp_free(net_tcp*);

error_code net_tcp_connect(net_tcp*, net_connect_opt*);
error_code net_tcp_listen(net_tcp*, net_address*);