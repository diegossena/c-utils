#include <sdk/udp.h>

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t* taskmanager) {
  queue_constructor(&this->_tasks);
  this->__socket = socket_new(SOCKET_TYPE_DGRAM);
  // service
  this->_service.context = this;
  this->_service.handle = (function_t)_udp_service;
  this->_service.destroy = (function_t)udp_deconstructor;
  this->_service.taskmanager = taskmanager;
  queue_push(&taskmanager->services, &this->_service.queue);
}
SDK_EXPORT udp_t* udp_new(taskmanager_t* taskmanager) {
  udp_t* this = memory_alloc0(sizeof(udp_t));
  udp_constructor(this, taskmanager);
  this->_service.destroy = (function_t)udp_free;
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
SDK_EXPORT void _udp_send_constructor(udp_send_t* this, udp_t* udp) {
  taskmanager_t* taskmanager = udp->_service.taskmanager;
  this->udp = udp;
  this->address.family = NET_FAMILY_IPV4;
  // __udp_tasks
  this->__udp_tasks.callback = (function_t)_udp_send_deconstructor;
  this->__udp_tasks.this = this;
  queue_push(&udp->_tasks, &this->__udp_tasks.queue);
  // task
  task_constructor(&this->_task, taskmanager);
  this->_task.handle = (function_t)_udp_send_startup;
  this->_task.destroy = (function_t)_udp_send_deconstructor;
  this->_task.context = this;
}
SDK_EXPORT void _udp_send_deconstructor(udp_send_t* this) {
  if (this->callback) {
    this->callback(this);
  }
  task_deconstructor(&this->_task);
  queue_remove(&this->__udp_tasks.queue);
}
SDK_EXPORT void _udp_send_startup(udp_send_t* this) {
  this->__updated_at = date_now();
  this->_task.handle = (function_t)_udp_send_task;
}