#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>

SDK_EXPORT void tcp_ip4_connect(tcp_t* this, u32 ip4, u16 net_port) {
  this->__socket = socket_new(NET_FAMILY_IPV4, SOCKET_TYPE_STREAM);
  if (this->__socket < 0) {
    closesocket(this->__socket);
  }
  this->address.family = NET_FAMILY_IPV4;
  this->address.ip4 = ip4;
  this->address.net_port = net_port;
  this->__task.handle = (function_t)__tcp_connect_handle;
}
SDK_EXPORT void tcp_connect(tcp_t* this, net_address_t* address) {
  this->__socket = socket_new(address->family, SOCKET_TYPE_STREAM);
  if (this->__socket < 0) {
    closesocket(this->__socket);
  }
  this->address = *address;
  this->__task.handle = (function_t)__tcp_connect_handle;
}
SDK_EXPORT void __tcp_connect_handle(tcp_t* this) {
  int error_code = connect(this->__socket, (SOCKADDR*)&this->address, sizeof(this->address));
  if (error_code == SOCKET_ERROR) {
    error_code = WSAGetLastError();
    if (error_code != WSAEWOULDBLOCK) {
      error("connect", error_code);
      tcp_free(this);
      return;
    }
  }
  this->__task.handle = (function_t)__tcp_connecting_handle;
  this->__updated_at = date_now();
}
SDK_EXPORT void __tcp_connecting_handle(tcp_t* this) {
  static struct timeval timeout = { 0, 0 };
  fd_set writable = {};
  FD_SET(this->__socket, &writable);
  i32 error_code = select(__net_max_fd, 0, &writable, 0, &timeout);
  if (error_code > 0) {
    // connected
    this->__task.handle = (function_t)tcp_free;
    this->onconnect(this);
    if (this->__task.handle == (function_t)tcp_free) {
      tcp_free(this);
    } else {
      this->__updated_at = date_now();
    }
    return;
  } else if (error_code == 0) {
    u64 deltaTime = date_now() - this->__updated_at;
    if (deltaTime > this->timeout) {
      error_code = ERR_ETIMEDOUT;
    } else {
      return;
    }
  } else {
    error_code = WSAGetLastError();
  }
  // onerror
  if (this->onerror) {
    this->onerror(this, error_code);
  } else {
    error("select", error_code);
  }
  tcp_free(this);
}
SDK_EXPORT void tcp_free(tcp_t* this) {
  closesocket((SOCKET)this->__socket);
  queue_remove((queue_t*)&this->__task);
  memory_free(this);
}

#endif