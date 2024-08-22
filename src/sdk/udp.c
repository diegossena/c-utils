#include <sdk/udp.h>

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t* taskmanager) {
  queue_constructor(&this->__tasks_destroy);
  this->__socket = socket_new(SOCKET_TYPE_DGRAM);
  this->__tasks = &taskmanager->tasks;
  // service
  this->_service.handle = (function_t)udp_service;
  this->_service.destroy = (function_t)udp_deconstructor;
  this->_service.context = this;
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
  this->_task.destroy = (function_t)udp_send_free;
}
SDK_EXPORT void udp_send_free(udp_send_t* this) {
  _udp_send_deconstructor(this);
  memory_free(this);
}
SDK_EXPORT void _udp_send_constructor(udp_send_t* this, udp_t* udp) {
  this->udp = udp;
  this->_task.context = this;
  this->_task.handle = (function_t)udp_send_startup;
  this->_task.destroy = (function_t)_udp_send_deconstructor;
  queue_push(udp->__tasks, &this->_task.queue);
}
SDK_EXPORT void _udp_send_deconstructor(udp_send_t* this) {
  if (this->callback) {
    this->callback(this);
  }
  queue_remove(&this->_task.queue);
  memory_free(this->data);
}
SDK_EXPORT void udp_send_startup(udp_send_t* this) {
  this->__data_ptr = this->data;
  this->__updated_at = date_now();
  this->_task.handle = (function_t)udp_send_task;
}