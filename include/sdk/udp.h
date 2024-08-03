#ifndef SDK_NET_UDP_H
#define SDK_NET_UDP_H

#include <sdk/types.h>
#include <sdk/taskmanager.h>
#include <sdk/error.h>

typedef struct udp_writer_t udp_writer_t;
typedef struct udp_message_t udp_message_t;

typedef void (*udp_onmessage_t)(udp_message_t*);

typedef struct udp_t {
  // public
  udp_onmessage_t onmessage;
  void* context;
  // private
  u64 __socket;
  task_t __task;
  queue_t __children;
} udp_t;

void udp_bind(udp_t* this, u16 port);

#endif