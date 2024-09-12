#include <sdk/tcp.h>

SDK_EXPORT tcp_t* tcp_new(taskmanager_t* taskmanager) {
  tcp_t* this = memory_alloc0(sizeof(tcp_t));
  _tcp_constructor(this, taskmanager);
  this->_task.destroy = (function_t)tcp_free;
  return this;
}
SDK_EXPORT void __tcp_onconnect(tcp_t* this, i8 result) {
  this->onend(this);
  if (result < 0) {
    _task_call_destroy(&this->_task);
  }
}
void __tcp_ontimeout(tcp_t* this) {
  this->error_code = ERR_ETIMEDOUT;
  PostQueuedCompletionStatus(this->_task.taskmanager->__iocp, -1, (ULONG_PTR)&this->_task, 0);
}