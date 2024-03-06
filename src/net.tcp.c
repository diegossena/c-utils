// net_tcp_on_connect, net_tcp_t, task_handle
#include "internal/net.tcp.h"
// memory_alloc0
#include "internal/memory.h"
// TODO: remove this line
#include "sdk/console.h"
// date_now
#include "sdk/date.h"
// app_global
#include "internal/application.h"

net_tcp_t* net_tcp_new() {
  net_tcp_t* this = memory_alloc0(sizeof(net_tcp_t));
  this->task.type = TASK_TCP_CLOSING;
  task_register(this);
  return this;
}
void net_tcp_set_context(net_tcp_t* this, const void* context) {
  this->stream.context = context;
}

void net_tcp_write(net_tcp_t* this, const byte* chunk, u64 length, net_tcp_on_write_cb callback) {
  this->task.type = TASK_TCP_WRITING;
  this->stream.writable = memory_alloc(length);
  this->stream.length = length;
  this->handle = (void*)callback;
  memory_copy(this->stream.writable, chunk, length);
}
void net_tcp_read(net_tcp_t* this, u64 length, net_tcp_on_read_cb callback) {
  this->task.type = TASK_TCP_READING;
  this->stream.length = length;
  this->stream.buffer_size = length > 0 ? length : MAX_BUFSIZ;
  this->stream.readable = memory_alloc(this->stream.buffer_size);
  this->handle = (void*)callback;
  this->stream.updatedAt = date_now();
}