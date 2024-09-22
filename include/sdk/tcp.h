#ifndef SDK_NET_TCP_H
#define SDK_NET_TCP_H

#include <sdk/types.h>
#include <sdk/taskmanager.h>
#include <sdk/net.h>
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

typedef void (*tcp_ondata_t)(tcp_t* this, byte_t* chunk, u32 length);
typedef void (*tcp_onend_t)(tcp_t*, error_code_t);

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

SDK_EXPORT tcp_t* tcp_new(taskmanager_t*);
SDK_EXPORT void tcp_free(tcp_t* this);
SDK_EXPORT void tcp_write(tcp_t* this, const byte_t* chunk, u64 length);
SDK_EXPORT void tcp_read(tcp_t* this, u64 length);

SDK_EXPORT void _tcp_constructor(tcp_t* this, taskmanager_t*);
SDK_EXPORT void _tcp_deconstructor(tcp_t* this);

SDK_EXPORT void __tcp_startup_task(tcp_t* this);
SDK_EXPORT void __tcp_onconnect(tcp_t* this, error_code_t);
SDK_EXPORT void __tcp_onwrite(tcp_t* this, error_code_t, u32 bytes);
SDK_EXPORT void __tcp_onread(tcp_t* this, error_code_t);

#endif