#ifndef SDK_UDP_H
#define SDK_UDP_H

#include <sdk/net.h>
#include <sdk/types.h>
#include <sdk/taskmanager.h>
#include <sdk/events.h>
#include <sdk/error.h>
#include <sdk/date.h>
#include <sdk/queue.h>
#include <sdk/assert.h>

/**
 * address.ip4: u64
 * data: byte_t*
 * length: u64
 * context?: any
 * callback?: udp_onsend_t
 */
typedef struct udp_send_t udp_send_t;
typedef struct udp_message_t udp_message_t;

typedef void (*udp_onmessage_t)(udp_message_t*);
typedef void (*udp_onsend_t)(udp_send_t*);

typedef struct udp_t {
  udp_onmessage_t onmessage;
  void* context;
  task_t _service;
  u64 __socket;
  queue_t* __tasks;
  queue_t __tasks_destroy; // queue_t<listener_t>
} udp_t;
typedef struct udp_message_t {
  udp_t* udp;
  byte_t* data;
  i32 length;
  net_address_t address;
} udp_message_t;
typedef struct udp_send_t {
  udp_t* udp;
  net_address_t address;
  byte_t* data;
  u64 length;
  void* context;
  udp_onsend_t callback;
  task_t _task;
  byte_t* __data_ptr;
  u64 __updated_at;
} udp_send_t;

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t*);
SDK_EXPORT void udp_deconstructor(udp_t* this);
SDK_EXPORT udp_t* udp_new(taskmanager_t*);
SDK_EXPORT void udp_free(udp_t* this);

SDK_EXPORT void udp_bind(udp_t* this, u16 port);
SDK_EXPORT void udp_send(udp_send_t* props);

SDK_EXPORT void udp_service(udp_t* this);

SDK_EXPORT udp_send_t* udp_send_new(udp_t* udp);
SDK_EXPORT void udp_send_free(udp_send_t* this);
SDK_EXPORT void _udp_send_constructor(udp_send_t* this, udp_t* udp);
SDK_EXPORT void _udp_send_deconstructor(udp_send_t* this);
SDK_EXPORT void udp_send_startup(udp_send_t* this);
SDK_EXPORT void udp_send_task(udp_send_t* this);

#endif