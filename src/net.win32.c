#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.h"

#include "sdk/error.h"

#include <winsock2.h>

void net_inicialize() {
  WSADATA wsaData;
  error_last = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_last) {
    error("WSAStartup", error_last);
    WSACleanup();
  }
}
void net_shutdown() {
  WSACleanup();
}

#endif