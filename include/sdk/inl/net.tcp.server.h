#ifdef SDK_NET_TCP_SERVER

#include <sdk/net/tcp/server.h>

net_tcp_server_t* net_tcp_server_new(application_t* app) {
  net_tcp_server_t* this = memory_alloc0(sizeof(net_tcp_server_t));
  this->app = app;
  this->__listener = (event_listener_t) {
    .callback = (listener_t)net_tcp_server_free,
    .context = this
  };
  emitter_on(&app->__tasks, &this->__listener);
  return this;
}
void net_tcp_client_write(net_tcp_client_t* this, const byte* chunk, u64 length, net_tcp_client_on_write_cb callback) {
  this->__listener.callback = (listener_t)net_tcp_client_write_handle;
  this->stream.writable = memory_alloc(length);
  this->stream.length = length;
  this->handle = (void*)callback;
  memory_copy(this->stream.writable, chunk, length);
}
void net_tcp_client_read(net_tcp_client_t* this, u64 length, net_tcp_client_on_read_cb callback) {
  this->__listener.callback = (listener_t)net_tcp_client_read_handle;
  this->stream.length = length;
  this->stream.buffer_size = length > 0
    ? length
    : MAX_BUFSIZ;
  this->stream.readable = memory_alloc(this->stream.buffer_size);
  this->handle = (void*)callback;
  this->stream.updatedAt = date_now();
}

#endif