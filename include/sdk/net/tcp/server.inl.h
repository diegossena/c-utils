#include <sdk/net/net.tcp.server.h>

net_tcp_server_t* net_tcp_server_new(application_t* app) {
  net_tcp_server_t* this = memory_alloc0(sizeof(net_tcp_server_t));
  this->app = app;
  this->task.type = TASK_TCP_SERVER_CLOSING;
  task_register(this, app);
  return this;
}
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