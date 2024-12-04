#include <sdk/tcp.h>

export tcp_t* tcp_new() {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  _tcp_constructor(this);
  this->_task.destroy = (function_t)tcp_free;
  return this;
}
export void tcp_free(tcp_t* this) {
  _tcp_deconstructor(this);
  memory_free(this);
}
export void _tcp_constructor(tcp_t* this) {
  this->timeout = 0;
  this->__timer = 0;
  // address
  this->address.family = NET_FAMILY_IPV4;
  // promise
  _promise_constructor(&this->_task);
  this->_task.callback = (task_callback_t)__tcp_startup;
  this->_task.destroy = (function_t)_tcp_deconstructor;
  this->_task.context = this;
  // startup
  err_t result = socket_new(&this->_task, SOCKET_TYPE_STREAM);
  if (result < 0) {
    // onerror
    this->onend(this->_task.context, result);
    this->_task.destroy(this->_task.context);
    return;
  }
  this->__socket = result;
  _promise_post(&this->_task, 0);
}
export void _tcp_deconstructor(tcp_t* this) {
  if (this->__timer) {
    timer_clear(this->__timer);
  }
  socket_free(this->__socket);
  _task_deconstructor(&this->_task);
}
export void __tcp_onwrite(tcp_t* this, error_t error_code, u32 bytes) {
  if (error_code) {
    this->onend(this, error_code);
    return _task_call_destroy(&this->_task);
  }
  this->__remaining -= bytes;
  if (this->__remaining == 0) {
    this->_task.callback = (task_callback_t)this->_task.destroy;
    this->onend(this, error_code);
  } else {
    if (this->__timer) {
      timer_set(this->__timer, this->timeout, 0);
    }
  }
}
export void __tcp_onconnect(tcp_t* this, error_t error_code) {
  this->onend(this, error_code);
  if (error_code) {
    _task_call_destroy(&this->_task);
  }
}
export void __tcp_ontimeout(tcp_t* this) {
  _socket_cancel(this->__socket);
}