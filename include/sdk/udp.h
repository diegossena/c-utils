#ifndef SDK_UDP_H
#define SDK_UDP_H

#include <sdk/net.h>
#include <sdk/types.h>
#include <sdk/task.h>
#include <sdk/events.h>
#include <sdk/error.h>
#include <sdk/date.h>
#include <sdk/queue.h>
#include <sdk/assert.h>

/**
 * address.ip4: u64
 * data: byte_t*
 * length: u64
 * ? context
 * udp_onsend_t? callback
 */
typedef struct udp_send_t udp_send_t;
typedef struct udp_message_t udp_message_t;

typedef void (*udp_onmessage_t)(udp_message_t*);
typedef void (*udp_onsend_t)(udp_send_t*);

typedef struct udp_t {
  udp_onmessage_t onmessage;
  void* context;
  task_t _promise;
  queue_t _tasks; // queue_t<callback_t>
  u64 __socket;
} udp_t;
typedef struct udp_message_t {
  udp_t* udp;
  char data[BUFFER_DEFAULT_SIZE];
  u32 length;
  net_address_t address;
} udp_message_t;
typedef struct udp_send_t {
  udp_t* udp;
  net_address_t address;
  char* data;
  u64 length;
  void* context;
  function_t callback;
  i32 error_code;
  task_t _task;
  callback_t __udp_tasks;
  u64 __updated_at;
} udp_send_t;

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t*);
SDK_EXPORT void udp_deconstructor(udp_t* this);
SDK_EXPORT udp_t* udp_new(taskmanager_t*);
SDK_EXPORT void udp_free(udp_t* this);

SDK_EXPORT void udp_bind(udp_t* this, u16 port);

SDK_EXPORT udp_send_t* udp_send_new(udp_t* udp);
SDK_EXPORT void udp_send_free(udp_send_t* this);

SDK_EXPORT void __udp_read(udp_t* this);

SDK_EXPORT void _udp_send_constructor(udp_send_t* this, udp_t* udp);
SDK_EXPORT void _udp_send_deconstructor(udp_send_t* this);
SDK_EXPORT void _udp_send_startup(udp_send_t* this);
SDK_EXPORT void _udp_send_task(udp_send_t* this);

SDK_EXPORT void __udp_onread(udp_t* this);

#endif