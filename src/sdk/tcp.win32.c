#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>

SDK_EXPORT void _tcp_deconstructor(tcp_t* this) {
  closesocket((SOCKET)this->__socket);
  _task_deconstructor(&this->_task);
}
SDK_EXPORT void tcp_free(tcp_t* this) {
  _tcp_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void tcp_write(tcp_t* this, const byte_t* chunk, u64 length) {
  this->_task.handle = (function_t)this->onend;
  this->__remaining = length;
  WSABUF data_buffer = { .buf = (void*)chunk, .len = length };
  OVERLAPPED overlapped = { 0 };
  DWORD bytesSent;
  i32 result = WSASend(this->__socket, &data_buffer, 1, &bytesSent, 0, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != WSAEWOULDBLOCK) {
      error("WSASend", result);
    }
  }
}
static char data[512] = "";
OVERLAPPED overlapped = { 0 };
SDK_EXPORT void tcp_read(tcp_t* this, u64 length) {
  this->_task.handle = (function_t)this->onend;
  this->__remaining = length;
  DWORD bytes;
  WSABUF data_buffer = { .len = 512, .buf = (char*)&data[0] };
  data_buffer.buf = data;
  data_buffer.len = sizeof(data);
  i32 result = WSARecv(this->__socket, &data_buffer, 1, &bytes, 0, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    if (result != WSAEWOULDBLOCK) {
      error("WSARecv", WSAGetLastError());
    }
  }
}
SDK_EXPORT void __tcp_startup_task(tcp_t* this) {
  i32 result;
  // bind
  struct sockaddr_in local_Address;
  local_Address.sin_family = AF_INET;
  local_Address.sin_port = htons(0);
  local_Address.sin_addr.s_addr = INADDR_ANY;
  result = bind(this->__socket, (SOCKADDR*)&local_Address, sizeof(local_Address));
  // ConnectionEx
  LPFN_CONNECTEX ConnectEx = 0;
  GUID guid = WSAID_CONNECTEX;
  DWORD bytes;
  result = WSAIoctl(this->__socket,
    SIO_GET_EXTENSION_FUNCTION_POINTER,
    &guid,
    sizeof(guid),
    &ConnectEx,
    sizeof(ConnectEx),
    &bytes,
    NULL,
    NULL
  );
  if (result == SOCKET_ERROR) {
    error("WSAIoctl", GetLastError());
  }
  OVERLAPPED overlapped = { 0 };
  result = ConnectEx(
    this->__socket, (SOCKADDR*)&this->address, sizeof(this->address), 0, 0,
    0, &overlapped
  );
  if (!result) {
    result = WSAGetLastError();
    if (result != ERR_IO_PENDING) {
      this->error_code = result;
      goto onerror;
    }
  }
  this->_task.handle = (function_t)this->onend;
  this->__updated_at = date_now();
  return _task_pending(&this->_task);
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}

#endif