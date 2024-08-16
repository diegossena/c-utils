#include <sdk/udp.h>

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t* taskmanager) {
  queue_constructor(&this->tasks_destroy);
  this->socket = socket_new(SOCKET_TYPE_DGRAM);
  this->tasks = &taskmanager->tasks;
  // service
  this->service.handle = (function_t)udp_service;
  this->service.destroy = (function_t)udp_deconstructor;
  this->service.context = this;
  queue_push(&taskmanager->services, &this->service.queue);
}
SDK_EXPORT void udp_deconstructor(udp_t* this) {
  closesocket((SOCKET)this->socket);
  callback_emit(&this->tasks_destroy);
  queue_remove(&this->service.queue);
}
SDK_EXPORT udp_t* udp_new(taskmanager_t* taskmanager) {
  udp_t* this = memory_alloc0(sizeof(udp_t));
  udp_constructor(this, taskmanager);
  this->service.destroy = (function_t)udp_free;
  return this;
}
SDK_EXPORT void udp_free(udp_t* this) {
  udp_deconstructor(this);
  free(this);
}

SDK_EXPORT udp_send_t* udp_send_new(udp_t* udp) {
  udp_send_t* this = memory_alloc0(sizeof(udp_send_t));
  this->_udp = udp;
  this->data = memory_alloc(length);
  this->__ptr = this->data;
  this->__remaining = length;
  memory_copy(this->data, data, length);
  // address
  this->address.family = NET_FAMILY_IPV4;
  this->address.ip4 = this->ip4;
  this->address.net_port = this->net_port;
  // task
  this->_task.context = this;
  this->_task.handle = (function_t)udp_writer_shutdown;
  this->_task.destroy = (function_t)udp_writer_shutdown;
  queue_push(udp->tasks, &this->_task.queue);
}
SDK_EXPORT void udp_send(udp_send_t* this) {
  udp->_task.handle = (function_t)udp_writer_startup;
}
SDK_EXPORT void udp_writer_startup(udp_send_t* this) {
  this->updated_at = date_now();
  this->_task.handle = (function_t)udp_writer_task;
}
SDK_EXPORT void udp_writer_shutdown(udp_send_t* this) {
  if (this->callback) {
    this->callback(this);
  }
  queue_remove(&this->_task.queue);
  memory_free(this->data);
  memory_free(this);
}