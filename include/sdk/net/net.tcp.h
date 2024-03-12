#pragma once
#include <sdk/error/error.h>
#include <sdk/application/task.h>
#include <sdk/stream.h>
typedef struct application_t application_t;

/** @brief Represents a TCP socket.
 *
 */
typedef struct net_tcp_t {
  task_t __task;
  stream_t __stream;
  u64 __socket;
  task_handle __handle;
  u64 __updatedAt;
} net_tcp_t;

typedef void (*net_tcp_on_connect_cb)(net_tcp_t* this);
typedef void (*net_tcp_on_write_cb)(net_tcp_t* this, const void* context);
typedef void (*net_tcp_on_read_cb)(net_tcp_t* this, const byte* data, u64 length, const void* context);
/** @brief Externally-defined function to create a net_tcp, provided by the consumer
 * of this library.
 * @param this A pointer which holds the created net_tcp object.
 * @returns True on successful creation; otherwise false.
 */
net_tcp_t* net_tcp_new(application_t*);
void net_tcp_free(net_tcp_t*);
/** @brief set socket ipv4 address to bind or connect
 * @param this net_tcp_t object
 * @param port port to listen or connect
 * @param host? host to connect
 */
error_code net_tcp_ip4_addr(net_tcp_t*, const char* host, u16 port);

void net_tcp_set_context(net_tcp_t*, const void* context);

void net_tcp_write(net_tcp_t*, const byte*, u64 length, net_tcp_on_write_cb);
void net_tcp_read(net_tcp_t*, u64 length, net_tcp_on_read_cb);

error_code net_tcp_ip4_connect(net_tcp_t*, const char* host, u16 port, net_tcp_on_connect_cb);

u64 net_tcp_create_socket(const net_addr_t* addr);
// task handlers
void net_tcp_connect_handle(net_tcp_t*);
void net_tcp_read_handle(net_tcp_t*);
void net_tcp_write_handle(net_tcp_t*);

#include <sdk/net/net.tcp.inl.h>
#include <sdk/net/net.tcp.win32.inl.h>