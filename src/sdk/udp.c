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
SDK_EXPORT void udp_send(udp_send_t* props) {
  assert(props->data.data);
  udp_writer_t* this = memory_alloc0(sizeof(udp_writer_t));
  this->callback = props->callback;
  this->context = props->context;
  this->udp = props->udp;
  this->buffer = memory_alloc(props->data.length);
  this->ptr = this->buffer;
  this->remaining = props->data.length;
  memory_copy(this->buffer, props->data.data, props->data.length);
  // address
  this->address.family = NET_FAMILY_IPV4;
  this->address.ip4 = props->ip4;
  this->address.net_port = props->net_port;
  // task
  this->task.context = this;
  this->task.handle = (function_t)udp_writer_startup;
  this->task.destroy = (function_t)udp_writer_shutdown;
  queue_push(props->udp->tasks, &this->task.queue);
}
SDK_EXPORT void udp_writer_startup(udp_writer_t* this) {
  this->updated_at = date_now();
  this->task.handle = (function_t)udp_writer_task;
}
SDK_EXPORT void udp_writer_shutdown(udp_writer_t* this) {
  if (this->callback) {
    this->callback(this);
  }
  queue_remove(&this->task.queue);
  memory_free(this->buffer);
  memory_free(this);
}