#include <sdk/udp.h>

export void udp_constructor(udp_t* this) {
  queue_constructor(&this->_tasks);
  this->__socket = socket_new(&this->_promise, SOCKET_TYPE_DGRAM);
  // task
  this->_promise.context = this;
  this->_promise.destroy = (function_t)udp_deconstructor;
  queue_push(&_global_promises, &this->_promise.queue);
  __udp_read(this);
}
export udp_t* udp_new() {
  udp_t* this = memory_alloc0(sizeof(udp_t));
  udp_constructor(this);
  this->_promise.destroy = (function_t)udp_free;
  return this;
}
export void udp_free(udp_t* this) {
  udp_deconstructor(this);
  free(this);
}
export udp_send_t* udp_send_new(udp_t* udp) {
  udp_send_t* this = memory_alloc0(sizeof(udp_send_t));
  _udp_send_constructor(this, udp);
  this->__udp_tasks.callback = (function_t)udp_send_free;
  this->_task.destroy = (function_t)udp_send_free;
  return this;
}
export void udp_send_free(udp_send_t* this) {
  _udp_send_deconstructor(this);
  memory_free(this);
}
export void udp_deconstructor(udp_t* this) {
  socket_free(this->__socket);
  callback_emit(&this->_tasks);
  queue_remove(&this->_promise.queue);
}
export void _udp_send_constructor(udp_send_t* this, udp_t* udp) {
  this->udp = udp;
  this->address.family = NET_FAMILY_IPV4;
  // __udp_tasks
  this->__udp_tasks.callback = (function_t)_udp_send_deconstructor;
  this->__udp_tasks.this = this;
  queue_push(&udp->_tasks, &this->__udp_tasks.queue);
  // task
  _task_constructor(&this->_task);
  this->_task.callback = (function_t)_udp_send_startup;
  this->_task.destroy = (function_t)_udp_send_deconstructor;
  this->_task.context = this;
}
export void _udp_send_deconstructor(udp_send_t* this) {
  if (this->callback) {
    this->callback(this->context);
  }
  _task_deconstructor(&this->_task);
  queue_remove(&this->__udp_tasks.queue);
}
export void _udp_send_startup(udp_send_t* this) {
  this->__updated_at = date_now();
  this->_task.callback = (function_t)_udp_send_task;
}