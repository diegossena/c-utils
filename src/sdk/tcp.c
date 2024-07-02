#include <sdk/tcp.h>

SDK_EXPORT tcp_t* tcp_new(taskmanager_t* taskmanager) {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  this->timeout = DEFAULT_TIMEOUT;
  this->__callback.callback = (listener_t)tcp_free;
  this->__callback.context = this;
  emitter_on(&taskmanager->__callback, &this->__callback);
  this->__destroy.callback = (listener_t)tcp_free;
  this->__destroy.context = this;
  emitter_on(&taskmanager->__destroy, &this->__destroy);
  return this;
}

SDK_EXPORT void tcp_write(tcp_t* this, const byte* chunk, u64 length) {
  this->__callback.callback = (listener_t)__tcp_write_handle;
  this->__buffer = memory_alloc(length);
  this->__ptr = this->__buffer;
  this->__remaining = length;
  memory_copy(this->__buffer, chunk, length);
  this->__updated_at = date_now();
}
SDK_EXPORT void tcp_read(tcp_t* this, u64 length) {
  this->__callback.callback = (listener_t)__tcp_read_handle;
  this->__remaining = length > 0 ? length : BUFSIZ;
  this->__buffer = memory_alloc(this->__remaining);
  this->__ptr = this->__buffer;
  this->__length = 0;
  this->__updated_at = date_now();
}