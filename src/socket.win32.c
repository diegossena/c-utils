#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/socket.h"
#include "sdk/console.h" // TODO: remove this line
#include "sdk/assert.h"

#include <winsock2.h>

bool __socket_inicialized = false;

error_code socket_startup() {
  if (!__socket_inicialized) {
    WSADATA wsaData;
    error_code code = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (code) {
      WSACleanup();
      error("WSAStartup", code);
      return code;
    }
    __socket_inicialized = true;
  }
  return ERR_SUCCESS;
}
void socket_shutdown() {
  if (__socket_inicialized) {
    WSACleanup();
    __socket_inicialized = false;
  }
}

error_code socket_constructor(net_socket* this, socket_options opt) {
  assert(*opt.host);
  assert(opt.port);

  error_code code;

  struct hostent* remoteHost = gethostbyname(opt.host);
  if (remoteHost == NULL) {
    code = WSAGetLastError();
    error("gethostbyname", code);
    return code;
  }

  struct in_addr addr;
  addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
  const char* host = (const char*)inet_ntoa(addr);

  struct sockaddr_in server_addr = {};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(opt.port);
  server_addr.sin_addr.s_addr = inet_addr(host);
  this->id = socket(server_addr.sin_family, SOCK_STREAM, IPPROTO_TCP);
  if (this->id == INVALID_SOCKET) {
    code = WSAGetLastError();
    closesocket(this->id);
    error("socket", code);
    return code;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long iMode = 1;
  if (opt.timeout) {
    code = ioctlsocket(this->id, FIONBIO, &iMode);
    if (code != NO_ERROR) {
      closesocket(this->id);
      error("ioctlsocket", code);
      return code;
    }
  }
  code = connect(this->id, (SOCKADDR*)&server_addr, sizeof(server_addr));
  if (opt.timeout) {
    if (code != WSAEWOULDBLOCK) {
      closesocket(this->id);
      error("connect", code);
      return code;
    }
    TIMEVAL timeval = { 0, opt.timeout * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(this->id, &readable);
    FD_ZERO(&writable);
    FD_SET(this->id, &writable);
    code = select(0, &readable, &writable, NULL, &timeval);
    if (code <= 0) {
      closesocket(this->id);
      error("select", ERR_ETIMEDOUT);
      return code;
    }
  } else if (code == SOCKET_ERROR) {
    closesocket(this->id);
    error("connect", ERR_ETIMEDOUT);
    return code;
  }
  return ERR_SUCCESS;
}
void socket_free(net_socket* this) {

}

#endif