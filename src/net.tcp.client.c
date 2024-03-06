// net_tcp_on_connect, net_tcp_client_t, task_handle
#include "internal/net.tcp.client.h"
// memory_alloc0
#include "internal/memory.h"
// TODO: remove this line
#include "sdk/console.h"
// date_now
#include "sdk/date.h"
// app_global
#include "internal/application.h"

void net_tcp_client_write(net_tcp_client_t* this, const byte* chunk, u64 length, net_tcp_client_on_write_cb callback) {
  this->task.type = TASK_TCP_CLIENT_WRITING;
  this->stream.writable = memory_alloc(length);
  this->stream.length = length;
  this->handle = (void*)callback;
  memory_copy(this->stream.writable, chunk, length);
}
void net_tcp_client_read(net_tcp_client_t* this, u64 length, net_tcp_client_on_read_cb callback) {
  this->task.type = TASK_TCP_CLIENT_READING;
  this->stream.length = length;
  this->stream.buffer_size = length > 0
    ? length
    : MAX_BUFSIZ;
  this->stream.readable = memory_alloc(this->stream.buffer_size);
  this->handle = (void*)callback;
  this->stream.updatedAt = date_now();
}