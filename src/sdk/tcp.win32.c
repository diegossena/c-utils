#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>

LPFN_CONNECTEX ConnectEx = 0;

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
  i32 result = WSASend(this->__socket, &data_buffer, 1, 0, 0, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != WSAEWOULDBLOCK) {
      error("WSASend", result);
    }
  }
}
SDK_EXPORT void tcp_read(tcp_t* this, u64 length) {
  this->_task.handle = (function_t)__tcp_onread;
  this->__remaining = length;
  DWORD flags;
  WSABUF data_buffer = { 0 };
  OVERLAPPED overlapped = { 0 };
  i32 result = WSARecv(this->__socket, &data_buffer, 1, 0, &flags, &overlapped, 0);
  if (result == SOCKET_ERROR) {
    result = WSAGetLastError();
    if (result != ERR_IO_PENDING) {
      console_log("!%x tcp_read %d", this->__socket, result);
      error("WSARecv", WSAGetLastError());
      this->_task.destroy(this->_task.context);
    }
  }
}
SDK_EXPORT void __tcp_onread(tcp_t* this) {
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
      if (result == WSAEWOULDBLOCK) {
        return tcp_read(this, this->__remaining);
      } else {
        this->error_code = result;
        this->onend(this);
        return _task_call_destroy(&this->_task);
      }
    } else {
      if (bytes > 0) {
        this->ondata(this, buffer, bytes);
        if (this->__remaining) {
          this->__remaining -= bytes;
        }
      } else {
        this->_task.handle = this->_task.destroy;
        this->onend(this);
        return _task_handle(&this->_task);
      }
    }
  } while (--count);
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
      return this->_task.destroy(this->_task.context);
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
  this->_task.handle = (function_t)this->onend;
  this->__updated_at = date_now();
  return _task_pending(&this->_task);
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}

#endif