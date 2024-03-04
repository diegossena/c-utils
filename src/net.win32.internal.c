#include "internal/platform.h"

#if PLATFORM_WINDOWS

// net_addr_t, net_address_resolve
#include "internal/net.h"
// assert
#include "sdk/assert.h"

#include <windows.h>

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