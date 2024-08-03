#ifndef SDK_NET_TCP_H
#define SDK_NET_TCP_H

#include <sdk/types.h>
#include <sdk/taskmanager.h>
#include <sdk/net.h>

typedef struct tcp_t tcp_t;

typedef void (*tcp_onconnect_t)(tcp_t*);
typedef void (*tcp_onwrite_t)(tcp_t*);
typedef void (*tcp_onread_t)(tcp_t*, const byte_t* data, u64 length);
typedef void (*tcp_onerror_t)(tcp_t*, error_code_t error_code);

/** @brief Represents a TCP socket.
 */
typedef struct tcp_t {
  task_t __task;
  // public
  void* context;
  tcp_onconnect_t onconnect;
  tcp_onwrite_t onwrite;
  tcp_onread_t onread;
  tcp_onerror_t onerror;
  u64 timeout;
  net_address_t address;
  // private
  u64 __socket;
  u64 __updated_at;
  // stream
  byte_t* __buffer;
  byte_t* __ptr;
  u64 __length;
  u64 __remaining;
} tcp_t;

SDK_EXPORT tcp_t* tcp_new(taskmanager_t*);
SDK_EXPORT void tcp_connect(tcp_t* this, net_address_t* address);
SDK_EXPORT void tcp_ip4_connect(tcp_t* this, u32 ip4, u16 port);
SDK_EXPORT void tcp_write(tcp_t*, const byte_t*, u64 length);
SDK_EXPORT void tcp_read(tcp_t*, u64 length);
SDK_EXPORT void tcp_free(tcp_t*);
SDK_EXPORT void __tcp_connect_handle(tcp_t* this);
SDK_EXPORT void __tcp_write_handle(tcp_t* this);
SDK_EXPORT void __tcp_read_handle(tcp_t* this);

#endif