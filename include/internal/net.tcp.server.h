#pragma once
// net_tcp_server_t
#include "sdk/net.tcp.server.h"
// task_t
#include "internal/task.h"
// net_addr_t
#include "internal/net.h"

typedef struct net_tcp_server_t {
  task_t task;
  u64 socket;
  u64 client_count;
} net_tcp_server_t;

void net_tcp_server_close_handle(net_tcp_server_t*);
void net_tcp_server_listen_handle(net_tcp_server_t*);