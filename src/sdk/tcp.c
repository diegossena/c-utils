#include <sdk/tcp.h>

SDK_EXPORT tcp_t* tcp_new(taskmanager_t* taskmanager) {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  _tcp_constructor(this, taskmanager);
  this->_task.destroy = (function_t)tcp_free;
  return this;
}
SDK_EXPORT void tcp_free(tcp_t* this) {
  _tcp_deconstructor(this);
  memory_free(this);
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
  timer_clear(this->__timer);
  _socket_free(this->__socket);
  _task_deconstructor(&this->_task);
}
SDK_EXPORT void __tcp_onwrite(tcp_t* this, u32 bytes) {
  this->__remaining -= bytes;
  if (this->__remaining == 0) {
    this->_task.handle = (task_handle_t)this->_task.destroy;
    this->onend(this);
  } else {
    timer_set(this->__timer, this->timeout, 0);
  }
}
SDK_EXPORT void __tcp_onconnect(tcp_t* this) {
  this->onend(this);
  if (this->error_code) {
    _task_call_destroy(&this->_task);
  }
}
SDK_EXPORT void __tcp_ontimeout(tcp_t* this) {
  this->error_code = ERR_ETIMEDOUT;
  _task_resolve(&this->_task, -1);
}