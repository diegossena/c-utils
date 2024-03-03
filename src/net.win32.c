#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.h"

#include "sdk/error.h"

#include <winsock2.h>

bool net_inicialized = false;

void net_inicialize() {
  WSADATA wsaData;
  error_last = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_last) {
    error("WSAStartup", error_last);
    WSACleanup();
  } else {
    net_inicialized = true;
  }
}
void net_shutdown() {
  if (net_inicialized) {
    WSACleanup();
  }
}

#endif