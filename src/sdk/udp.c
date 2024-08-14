#include <sdk/udp.h>

SDK_EXPORT void udp_constructor(udp_t* this, taskmanager_t* taskmanager) {
  queue_constructor(&this->tasks_destroy);
  this->socket = socket_new(AF_INET, SOCKET_TYPE_DGRAM);
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
  udp_writer_t* this = memory_alloc0(sizeof(udp_writer_t));
  this->address = props->address;
  this->callback = props->callback;
  this->context = props->context;
  this->__udp = props->udp;
  this->__task.context = this;
  this->__task.handle = (function_t)udp_writer_startup;
  this->__task.destroy = (function_t)udp_writer_shutdown;
  queue_unshift(&props->udp->service.queue, &this->__task.queue);
  this->__buffer = memory_alloc0(props->data->length);
  this->ptr = this->__buffer;
  this->remaining = props->data->length;
  memory_copy(this->__buffer, props->data->data, props->data->length);
}
SDK_EXPORT void udp_writer_startup(udp_writer_t* this) {
  this->updated_at = date_now();
  this->__task.handle = (function_t)udp_writer_task;
}
SDK_EXPORT void udp_writer_shutdown(udp_writer_t* this) {
  queue_remove(&this->__task.queue);
  memory_free(this->__buffer);
  memory_free(this);
}