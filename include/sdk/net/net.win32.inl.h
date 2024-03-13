#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/net/net.h>
#include <sdk/error/error.h>
#include <winsock2.h>

static bool __net_inicialized = false;

void net_startup() {
  WSADATA wsaData;
  error_last = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_last) {
    error("WSAStartup", error_last);
    WSACleanup();
  } else {
    __net_inicialized = true;
  }
}
void net_shutdown() {
  if (__net_inicialized) {
    WSACleanup();
  }
}

u32 net_host_resolve(const char* host) {
  if (host) {
    if (*host >= '0' && *host <= '9') {
      return inet_addr(host);
    } else {
      struct hostent* remote_host = gethostbyname(host);
      if (remote_host) {
        return *(u32*)remote_host->h_addr_list[0];
      }
      error("gethostbyname", WSAGetLastError());
    }
  }
  return 0;
}

#endif