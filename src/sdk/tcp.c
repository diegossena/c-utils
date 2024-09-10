#include <sdk/tcp.h>

SDK_EXPORT tcp_t* tcp_new(taskmanager_t* taskmanager) {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  _tcp_constructor(this, taskmanager);
  // task
  this->_task.destroy = (function_t)tcp_free;
  return this;
}
SDK_EXPORT void _tcp_constructor(tcp_t* this, taskmanager_t* taskmanager) {
  this->timeout = NET_DEFAULT_TIMEOUT;
  this->error_code = 0;
  // address
  this->address.family = NET_FAMILY_IPV4;
  // task
  _task_constructor(&this->_task, taskmanager);
  this->_task.handle = (function_t)__tcp_startup_task;
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