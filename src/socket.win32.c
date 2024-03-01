#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/socket.h"
#include "sdk/assert.h"
#include "sdk/console.h" // TODO: remove this line

#include "base/memory.h"

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

  struct hostent* remoteHost = gethostbyname(opt.host);
  if (remoteHost == NULL) {
    error("gethostbyname", WSAGetLastError());
    return error_last;
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
    error("socket", WSAGetLastError());
    closesocket(this->id);
    return error_last;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long iMode = 1;
  if (opt.timeout) {
    error_last = ioctlsocket(this->id, FIONBIO, &iMode);
    if (error_last != NO_ERROR) {
      error("ioctlsocket", error_last);
      closesocket(this->id);
      return error_last;
    }
  }
  error_last = connect(this->id, (SOCKADDR*)&server_addr, sizeof(server_addr));
  if (opt.timeout) {
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      closesocket(this->id);
      return error_last;
    }
    TIMEVAL timeval = { 0, opt.timeout * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(this->id, &readable);
    FD_ZERO(&writable);
    FD_SET(this->id, &writable);
    error_last = select(0, &readable, &writable, NULL, &timeval);
    if (error_last <= 0) {
      error("select", ERR_ETIMEDOUT);
      closesocket(this->id);
      return error_last;
    }
  } else if (error_last == SOCKET_ERROR) {
    error("connect", ERR_ETIMEDOUT);
    closesocket(this->id);
    return error_last;
  }
  return ERR_SUCCESS;
}
void socket_free(net_socket* this) {
  closesocket(this->id);
}

error_code socket_write(net_socket* this, const byte* chunk, u32 length) {
  error_code code = send(this->id, chunk, length, 0);
  if (code == SOCKET_ERROR) {
    error("send", WSAGetLastError());
    return code;
  }
  return ERR_SUCCESS;
}
i32 socket_read(net_socket* this, byte* buffer, u32 length) {
  i32 received = recv(this->id, buffer, length, 0);
  if (received < 0) {
    error("recv", WSAGetLastError());
  }
  return received;
}

#endif