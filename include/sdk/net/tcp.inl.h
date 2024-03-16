#include <sdk/net/tcp.h>
#include <sdk/memory.h>

net_tcp_t* net_tcp_new(application_t* app) {
  net_tcp_t* this = memory_alloc0(sizeof(net_tcp_t));
  this->__task.type = TASK_TCP_CLOSING;
  task_register(this, app);
  return this;
}
void net_tcp_set_context(net_tcp_t* this, const void* context) {
  this->__stream.context = context;
}

void net_tcp_write(net_tcp_t* this, const byte* chunk, u64 length, net_tcp_on_write_cb callback) {
  this->__task.type = TASK_TCP_WRITING;
  this->__stream.writable = memory_alloc(length);
  this->__stream.length = length;
  this->__handle = (void*)callback;
  memory_copy(this->__stream.writable, chunk, length);
}
void net_tcp_read(net_tcp_t* this, u64 length, net_tcp_on_read_cb callback) {
  this->__task.type = TASK_TCP_READING;
  this->__stream.length = length;
  this->__stream.buffer_size = length > 0 ? length : MAX_BUFSIZ;
  this->__stream.readable = memory_alloc(this->__stream.buffer_size);
  this->__handle = (void*)callback;
}