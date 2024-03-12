#pragma once
#include <sdk/types.h>
#include <sdk/application/task.h>
#include <sdk/stream.h>
#include <sdk/net/net.h>
#include <sdk/net/net.tcp.server.h>

/** @brief Represents a TCP server connection.
 *
 */
typedef struct net_tcp_client_t {
  // extends net_tcp_t
  task_t task;
  stream_t stream;
  u64 socket;
  task_handle handle;
  // fields
  net_addr_t addr;
  net_tcp_server_t* server;
} net_tcp_client_t;
// callbacks
typedef void (*net_tcp_client_on_write_cb)(net_tcp_client_t* this, const void* context);
typedef void (*net_tcp_client_on_read_cb)(net_tcp_client_t* this, const byte* data, u64 length, const void* context);
// methods
void net_tcp_client_write(net_tcp_client_t* this, const byte* chunk, u64 length, net_tcp_client_on_write_cb callback);
void net_tcp_client_read(net_tcp_client_t* this, u64 length, net_tcp_client_on_read_cb callback);
// task handlers
void net_tcp_client_free(net_tcp_client_t*);
void net_tcp_client_read_handle(net_tcp_client_t*);
void net_tcp_client_write_handle(net_tcp_client_t*);

#include <sdk/net/net.tcp.client.inl.h>
#include <sdk/net/net.tcp.client.win32.inl.h>