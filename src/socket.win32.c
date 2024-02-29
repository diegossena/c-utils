#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/socket.h"
#include "sdk/console.h" // TODO: remove this line

#include <winsock2.h>

bool __socket_inicialized = false;

error_code socket_startup() {
  if (!__socket_inicialized) {
    WSADATA wsaData;
    i32 code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (code) {
      WSACleanup();
      error("WSAStartup", code);
      return code;
    }
    __socket_inicialized = true;
  }
  return 0;
}
void socket_shutdown() {
  if (__socket_inicialized) {
    WSACleanup();
    __socket_inicialized = false;
  }
}

void socket_constructor(net_socket* sock, socket_options opt) {
  console_log("opt.host=%s", opt.host);
}

#endif