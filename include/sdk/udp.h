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

typedef struct udp_writer_t udp_writer_t;
typedef struct udp_message_t udp_message_t;

typedef void (*udp_onmessage_t)(udp_message_t*);
typedef void (*udp_onwriter_t)(udp_writer_t*);

typedef struct udp_t {
  udp_onmessage_t onmessage;
  void* context;
  u64 socket;
  task_t service;
  queue_t* tasks;
  queue_t tasks_destroy; // queue_t<listener_t>
} udp_t;
typedef struct udp_message_t {
  udp_t* udp;
  byte_t* data;
  i32 length;
  net_address_t address;
} udp_message_t;
typedef struct udp_writer_t {
  void* context;
  net_address_t address;
  udp_onwriter_t callback;
  u64 updated_at;
  udp_t* udp;
  task_t task;
  // stream
  byte_t* buffer;
  byte_t* ptr;
  u64 __length;
  u64 remaining;
} udp_writer_t;
typedef struct udp_send_t {
  udp_t* udp;
  u32 ip4;
  u16 net_port;
  buffer_t data;
  udp_onwriter_t callback;
  void* context;
} udp_send_t;

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t*);
SDK_EXPORT void udp_deconstructor(udp_t* this);
SDK_EXPORT udp_t* udp_new(taskmanager_t*);
SDK_EXPORT void udp_free(udp_t* this);

SDK_EXPORT void udp_bind(udp_t* this, u16 port);
SDK_EXPORT void udp_send(udp_send_t* props);

SDK_EXPORT void udp_service(udp_t* this);

SDK_EXPORT void udp_writer_startup(udp_writer_t* this);
SDK_EXPORT void udp_writer_task(udp_writer_t* this);
SDK_EXPORT void udp_writer_shutdown(udp_writer_t* this);

#endif