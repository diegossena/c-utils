#include <sdk/promise.h>

queue_t _global_promises;

SDK_EXPORT void _promise_startup() {
  queue_constructor(&_global_promises);
  __promise_startup();
}
SDK_EXPORT void _promise_constructor(task_t* this) {
  queue_push(&_global_promises, &this->queue);
  ++__global_tasks_count;
}
SDK_EXPORT void _promise_deconstructor(task_t* this) {
  _task_deconstructor(this);
}