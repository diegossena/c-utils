#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>

LPFN_CONNECTEX ConnectEx = 0;

SDK_EXPORT void tcp_write(tcp_t* this, const byte_t* chunk, u64 length) {
  this->__remaining = length;
  WSABUF data_buffer = { length, (char*)chunk };
  OVERLAPPED overlapped = { 0 };
  i32 result = WSASend(this->__socket, &data_buffer, 1, 0, 0, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != WSAEWOULDBLOCK) {
      this->error_code = result;
      this->onend(this);
      return _task_call_destroy(&this->_task);
    }
  }
  this->_task.handle = (task_handle_t)__tcp_onwrite;
}
SDK_EXPORT void tcp_read(tcp_t* this, u64 length) {
  this->__remaining = length;
  DWORD bytes, flags = 0;
  WSABUF data_buffer = { 0 };
  OVERLAPPED overlapped = { 0 };
  i32 result = WSARecv(this->__socket, &data_buffer, 1, &bytes, &flags, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != ERR_IO_PENDING) {
      this->error_code = result;
      this->onend(this);
      return _task_call_destroy(&this->_task);
    }
  }
  this->_task.handle = (task_handle_t)__tcp_onread;
}
SDK_EXPORT void __tcp_onread(tcp_t* this) {
  if (this->error_code) {
    this->onend(this);
    return _task_call_destroy(&this->_task);
  }
  // Do nonblocking reads until the buffer is empty
  u8 count = 32;
  char buffer[BUFFER_DEFAULT_SIZE];
  WSABUF data_buffer = {
    this->__remaining ? this->__remaining : BUFFER_DEFAULT_SIZE,
    buffer
  };
  DWORD bytes, flags;
  do {
    i32 result = WSARecv(this->__socket, &data_buffer, 1, &bytes, &flags, 0, 0);
    if (result == SOCKET_ERROR) {
      result = WSAGetLastError();
      if (result == WSAEWOULDBLOCK)
        break;
      // onerror
      this->error_code = result;
      this->onend(this);
      return _task_call_destroy(&this->_task);
    }
    if (!bytes)
      goto onend;
    this->ondata(this, buffer, bytes);
    if (this->__remaining) {
      this->__remaining -= bytes;
      if (!this->__remaining)
        goto onend;
    }
  } while (--count);
  timer_set(this->__timer, this->timeout, 0);
  return tcp_read(this, this->__remaining);
onend:
  this->_task.handle = (task_handle_t)this->_task.destroy;
  _task_handle(&this->_task);
  this->onend(this);
}
SDK_EXPORT void __tcp_startup_task(tcp_t* this) {
  i32 result;
  // bind
  struct sockaddr_in local_Address;
  local_Address.sin_family = AF_INET;
  local_Address.sin_port = htons(0);
  local_Address.sin_addr.s_addr = INADDR_ANY;
  result = bind(this->__socket, (SOCKADDR*)&local_Address, sizeof(local_Address));
  if (!ConnectEx) {
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
      return _task_call_destroy(&this->_task);
    }
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
  this->_task.handle = (task_handle_t)__tcp_onconnect;
  u64 start = date_now();
  this->__timer = timer_new((function_t)__tcp_ontimeout, this, this->timeout, 0);
  return _task_promise(&this->_task);
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}

#endif