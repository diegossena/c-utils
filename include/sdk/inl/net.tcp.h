#ifdef SDK_NET_H

#include <sdk/memory.h>
#include <sdk/net/tcp.h>

net_tcp_t* net_tcp_new(application_t* app) {
  net_tcp_t* this = memory_alloc0(sizeof(net_tcp_t));
  this->__listener = (event_listener_t) {
    .callback = (listener_t)net_tcp_free,
    .context = this
  };
  emitter_on(&app->__tasks, &this->__listener);
  return this;
}

void net_tcp_write(net_tcp_t* this, const byte* chunk, u64 length, net_tcp_on_write_cb callback) {
  this->__listener.callback = (listener_t)net_tcp_write_handle;
  this->__handle = (fn_any_t)callback;
  this->__stream.writable = memory_alloc(length);
  this->__stream.length = length;
  memory_copy(this->__stream.writable, chunk, length);
}
void net_tcp_read(net_tcp_t* this, u64 length, net_tcp_on_read_cb callback) {
  this->__listener.callback = (listener_t)net_tcp_read_handle;
  this->__handle = (fn_any_t)callback;
  this->__stream.length = length;
  this->__stream.buffer_size = length > 0 ? length : MAX_BUFSIZ;
  this->__stream.readable = memory_alloc(this->__stream.buffer_size);
}

#endif