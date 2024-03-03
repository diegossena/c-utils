#pragma once

#include <sdk/types.h>

typedef enum net_family {
  NET_IPV4 = 2,
  NET_IPV6 = 23
} net_family;

interface net_address_t {
  net_family family;
  u16 port;
} net_address_t;

interface net_connect_opt {
  net_address_t adress;
  const char* host;
  u16 timeout;
} net_connect_opt;

void net_inicialize();