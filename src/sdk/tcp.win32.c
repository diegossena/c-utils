#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>
#include <windows.h>

SDK_EXPORT void _tcp_deconstructor(tcp_t* this) {
  closesocket((SOCKET)this->__socket);
  task_deconstructor(&this->_task);
}
SDK_EXPORT void tcp_free(tcp_t* this) {
  _tcp_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void __tcp_startup_task(tcp_t* this) {
  this->__socket = socket_new(SOCKET_TYPE_STREAM);
  if (this->__socket < 0) {
    this->error_code = this->__socket;
    this->onend(this->_task.context);
    goto onerror;
  }
  i32 error_code = connect(this->__socket, (SOCKADDR*)&this->address, sizeof(this->address));
  if (error_code == SOCKET_ERROR) {
    error_code = WSAGetLastError();
    if (error_code != ERR_EWOULDBLOCK) {
      this->error_code = error_code;
      goto onerror;
    }
  }
  this->_task.handle = (function_t)__tcp_connect_task;
  this->__updated_at = date_now();
  return;
onerror:
  this->onend(this->_task.context);
  this->_task.destroy(this->_task.context);
}
SDK_EXPORT void __tcp_connect_task(tcp_t* this) {
  struct timeval timeout = { 0, 0 };
  fd_set writable = {};
  FD_SET(this->__socket, &writable);
  i32 error_code = select(__net_max_fd, 0, &writable, 0, &timeout);
  // onfail
  if (error_code == 0) {
    u64 deltaTime = date_now() - this->__updated_at;
    if (deltaTime > this->timeout) {
      this->error_code = ERR_ETIMEDOUT;
      goto onend;
    }
    goto awaiting;
  }
  // onsuccess
  if (error_code > 0) {
    goto onend;
  }
  // onerror
  this->error_code = WSAGetLastError();
onend:
  this->_task.handle = this->_task.destroy;
  this->onend(this->_task.context);
  if (this->_task.handle == this->_task.destroy) {
    this->_task.destroy(this->_task.context);
  }
awaiting:
  return;
}

SDK_EXPORT void __tcp_write_task(tcp_t* this) {
  i32 sent = send(this->__socket, this->__buffer, this->__remaining, 0);
  if (sent > 0) {
    this->__updated_at = date_now();
    this->__buffer += sent;
    this->__remaining -= sent;
    if (this->__remaining) {
      goto awaiting;
    }
  } else if (sent < 0) {
    sent = WSAGetLastError();
    if (sent == WSAEWOULDBLOCK)
      goto awaiting;
    this->error_code = sent;
  } else {
    u64 elapsed = date_now() - this->__updated_at;
    if (elapsed > this->timeout) {
      this->error_code = ERR_ETIMEDOUT;
    } else {
      goto awaiting;
    }
  }
onend:
  this->_task.handle = this->_task.destroy;
  this->onend(this->_task.context);
  if (this->_task.handle == this->_task.destroy) {
    this->_task.destroy(this->_task.context);
  }
awaiting:
  return;
}
SDK_EXPORT void __tcp_read_task(tcp_t* this) {
  buffer_default_t buffer;
  u64 buffer_size = this->__remaining ? this->__remaining : BUFFER_DEFAULT_SIZE;
  i32 length = recv(this->__socket, buffer, buffer_size, 0);
  if (length > 0) {
    this->ondata(this, buffer, length);
    if (this->__remaining) {
      this->__remaining -= length;
      if (!this->__remaining) {
        goto onend;
      }
    }
    this->__updated_at = date_now();
    goto awaiting;
  }
  if (length == 0) {
    goto onend;
  }
  u64 elapsed = date_now() - this->__updated_at;
  if (elapsed > this->timeout) {
    this->error_code = ERR_ETIMEDOUT;
    goto onend;
  }
  length = WSAGetLastError();
  if (length == WSAEWOULDBLOCK) {
    goto awaiting;
  }
  this->error_code = length;
onend:
  this->_task.handle = this->_task.destroy;
  this->onend(this->_task.context);
  if (this->_task.handle == this->_task.destroy) {
    this->_task.destroy(this->_task.context);
  }
awaiting:
  return;
}

#endif