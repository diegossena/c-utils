#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>

LPFN_CONNECTEX ConnectEx = 0;

SDK_EXPORT void tcp_free(tcp_t* this) {
  _tcp_deconstructor(this);
  memory_free(this);
}
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
SDK_EXPORT void _tcp_constructor(tcp_t* this, taskmanager_t* taskmanager) {
  this->timeout = NET_DEFAULT_TIMEOUT;
  this->error_code = 0;
  // address
  this->address.family = NET_FAMILY_IPV4;
  // task
  _task_constructor(&this->_task, taskmanager);
  this->_task.handle = (task_handle_t)__tcp_startup_task;
  this->_task.destroy = (function_t)_tcp_deconstructor;
  this->_task.context = this;
  // startup
  this->__socket = socket_new(&this->_task, SOCKET_TYPE_STREAM);
  if (this->__socket < 0) {
    this->error_code = this->__socket;
    this->onend(this->_task.context);
    goto onerror;
  }
  return;
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}
SDK_EXPORT void _tcp_deconstructor(tcp_t* this) {
  console_log("%llu %d _tcp_deconstructor", date_now(), this->__socket);
  WINBOOL result = DeleteTimerQueueTimer(0, this->__timer, 0);
  if (result == 0) {
    error("DeleteTimerQueueTimer", GetLastError());
  }
  closesocket((SOCKET)this->__socket);
  _task_deconstructor(&this->_task);
}
SDK_EXPORT void __tcp_onwrite(tcp_t* this, u32 bytes) {
  this->__remaining -= bytes;
  if (this->__remaining == 0) {
    this->_task.handle = (task_handle_t)this->_task.destroy;
    this->onend(this);
  } else {
    ChangeTimerQueueTimer(0, this->__timer, this->timeout, 0);
  }
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
  OVERLAPPED overlapped = { 0 };
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
  console_log("%llu %d ChangeTimerQueueTimer", date_now(), this->__socket);
  ChangeTimerQueueTimer(0, this->__timer, this->timeout, 0);
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
  result = CreateTimerQueueTimer(&this->__timer, 0, (WAITORTIMERCALLBACK)__tcp_ontimeout, this, this->timeout, 0, 0);
  if (result == 0) {
    error("CreateTimerQueueTimer", GetLastError());
  }
  return _task_promise(&this->_task);
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}

#endif