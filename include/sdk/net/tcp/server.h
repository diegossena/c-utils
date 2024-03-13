#ifndef SDK_NET_TCP_SERVER_H
#define SDK_NET_TCP_SERVER_H

#include <sdk/net/tcp.h>
#include <sdk/error/error.h>

typedef struct net_tcp_client_t net_tcp_client_t;

/** @brief Represents a TCP stream or TCP server.
 *
 */
typedef struct net_tcp_server_t {
  task_t task;
  application_t* app;
  u64 socket;
  u64 client_count;
  net_tcp_on_connection_cb handle;
} net_tcp_server_t;
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

typedef void (*net_tcp_on_connection_cb)(net_tcp_client_t*);
typedef void (*net_tcp_client_on_write_cb)(net_tcp_client_t* this, const void* context);
typedef void (*net_tcp_client_on_read_cb)(net_tcp_client_t* this, const byte* data, u64 length, const void* context);

/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp_server_t* net_tcp_server_new(application_t*);
void net_tcp_server_free(net_tcp_server_t*);

error_code net_tcp_server_ip4_listen(net_tcp_server_t*, u16 port, net_tcp_on_connection_cb);

void net_tcp_client_write(net_tcp_client_t* this, const byte* chunk, u64 length, net_tcp_client_on_write_cb callback);
void net_tcp_client_read(net_tcp_client_t* this, u64 length, net_tcp_client_on_read_cb callback);

void net_tcp_server_close_handle(net_tcp_server_t*);
void net_tcp_server_listen_handle(net_tcp_server_t*);
void net_tcp_client_free(net_tcp_client_t*);
void net_tcp_client_read_handle(net_tcp_client_t*);
void net_tcp_client_write_handle(net_tcp_client_t*);

#include <sdk/net/tcp.server.win32.inl.h>
#include <sdk/net/net.tcp.server.inl.h>

#endif