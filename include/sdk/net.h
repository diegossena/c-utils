#pragma once

#include <sdk/types.h>

#define NET_ADDRESS_FIELDS \
  net_family family; \
  u16 port;

typedef enum net_family {
  NET_IPV4 = 2,
  NET_IPV6 = 23
} net_family;

interface net_address {
  NET_ADDRESS_FIELDS;
} net_address;

interface net_connect_opt {
  NET_ADDRESS_FIELDS;
  const char* host;
  u16 timeout;
} net_connect_opt;