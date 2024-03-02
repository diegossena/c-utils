#pragma once

#include <sdk/types.h>

class net_socket {
  // private
  u64 id;
  // public
  const char* host;
  u16 port, timeout;
} net_socket;