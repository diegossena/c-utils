#ifndef TCP_H
#define TCP_H

#include <sdk/types.h>
#include <sdk/net.h>
#include <sdk/promise.h>
#include <sdk/date.h>
#include <sdk/timer.h>

/**
 * @brief Represents a TCP socket.
 *
 * - net_address_t address
 *
 * - tcp_onend_t onend
 *
 * - ? context
 *
 * - tcp_ondata_t? ondata
 *
 * - u64? timeout
 */
typedef struct tcp_t tcp_t;

typedef void (*tcp_ondata_t)(tcp_t* this, char* chunk, u32 length);
typedef void (*tcp_onend_t)(tcp_t*, error_t);

typedef struct tcp_t {
  task_t _task;
  void* context;
  tcp_ondata_t ondata;
  tcp_onend_t onend;
  u64 timeout;
  net_address_t address;
  tcp_onend_t __callback;
  u64 __socket;
  timer_t __timer;
  // stream
  u64 __remaining;
} tcp_t;

export tcp_t* tcp_new();
export void tcp_free(tcp_t* this);
export void tcp_write(tcp_t* this, const char* chunk, u64 length);
export void tcp_read(tcp_t* this, u64 length);

export void _tcp_constructor(tcp_t* this);
export void _tcp_deconstructor(tcp_t* this);

export void __tcp_startup(tcp_t* this);
export void __tcp_onconnect(tcp_t* this, error_t);
export void __tcp_onwrite(tcp_t* this, error_t, u32 bytes);
export void __tcp_onread(tcp_t* this, error_t);
export void __tcp_ontimeout(tcp_t* this);

#endif