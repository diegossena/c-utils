#include <sdk/tcp.h>

SDK_EXPORT void _tcp_constructor(tcp_t* this, taskmanager_t* taskmanager) {
  this->timeout = NET_DEFAULT_TIMEOUT;
  this->error_code = 0;
  // address
  this->address.family = NET_FAMILY_IPV4;
  // task
  task_constructor(&this->_task, taskmanager);
  this->_task.handle = (function_t)__tcp_startup_task;
  this->_task.destroy = (function_t)_tcp_deconstructor;
  this->_task.context = this;
}
SDK_EXPORT tcp_t* tcp_new(taskmanager_t* taskmanager) {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  _tcp_constructor(this, taskmanager);
  // task
  this->_task.destroy = (function_t)tcp_free;
  return this;
}
SDK_EXPORT void tcp_write(tcp_t* this, const byte_t* chunk, u64 length) {
  this->_task.handle = (function_t)__tcp_write_task;
  this->__buffer = chunk;
  this->__remaining = length;
  this->__updated_at = date_now();
}
SDK_EXPORT void tcp_read(tcp_t* this, u64 length) {
  this->_task.handle = (function_t)__tcp_read_task;
  this->__remaining = length;
  this->__updated_at = date_now();
}
