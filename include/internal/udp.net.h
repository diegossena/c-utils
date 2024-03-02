#pragma once

#include "internal/application.h"
#include "internal/stream.h"
#include "internal/net.h"

// Represents a UDP stream or UDP server.
typedef struct net_udp {
  HANDLE_FIELDS;
  SOCKET_FIELDS;
  /*
   * Number of bytes queued for sending. This field strictly shows how much
   * information is currently queued.
   */
  u64 send_queue_size;
  /*
   * Number of send requests currently in the queue awaiting to be processed.
   */
  u64 send_queue_count;
} net_udp;