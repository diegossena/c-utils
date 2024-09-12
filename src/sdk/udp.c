#include <sdk/udp.h>

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t* taskmanager) {
  queue_constructor(&this->_tasks);
  this->__socket = socket_new(&this->_task, SOCKET_TYPE_DGRAM);
  // task
  this->_task.context = this;
  this->_task.destroy = (function_t)udp_deconstructor;
  this->_task.taskmanager = taskmanager;
  queue_push(&taskmanager->promises, &this->_task.queue);
  __udp_read(this);
}
SDK_EXPORT udp_t* udp_new(taskmanager_t* taskmanager) {
  udp_t* this = memory_alloc0(sizeof(udp_t));
  udp_constructor(this, taskmanager);
  this->_task.destroy = (function_t)udp_free;
  return this;
}
SDK_EXPORT void udp_free(udp_t* this) {
  udp_deconstructor(this);
  free(this);
}
SDK_EXPORT udp_send_t* udp_send_new(udp_t* udp) {
  udp_send_t* this = memory_alloc0(sizeof(udp_send_t));
  _udp_send_constructor(this, udp);
  this->__udp_tasks.callback = (function_t)udp_send_free;
  this->_task.destroy = (function_t)udp_send_free;
  return this;
}
SDK_EXPORT void udp_send_free(udp_send_t* this) {
  _udp_send_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void udp_deconstructor(udp_t* this) {
  _socket_free(this->__socket);
  callback_emit(&this->_tasks);
  queue_remove(&this->_task.queue);
}
SDK_EXPORT void _udp_send_constructor(udp_send_t* this, udp_t* udp) {
  taskmanager_t* taskmanager = udp->_task.taskmanager;
  this->udp = udp;
  this->address.family = NET_FAMILY_IPV4;
  // __udp_tasks
  this->__udp_tasks.callback = (function_t)_udp_send_deconstructor;
  this->__udp_tasks.this = this;
  queue_push(&udp->_tasks, &this->__udp_tasks.queue);
  // task
  _task_constructor(&this->_task, taskmanager);
  this->_task.handle = (function_t)_udp_send_startup;
  this->_task.destroy = (function_t)_udp_send_deconstructor;
  this->_task.context = this;
}
SDK_EXPORT void _udp_send_deconstructor(udp_send_t* this) {
  if (this->callback) {
    this->callback(this->context);
  }
  _task_deconstructor(&this->_task);
  queue_remove(&this->__udp_tasks.queue);
}
SDK_EXPORT void _udp_send_startup(udp_send_t* this) {
  this->__updated_at = date_now();
  this->_task.handle = (function_t)_udp_send_task;
}