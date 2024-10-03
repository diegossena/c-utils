#include <sdk/promise.h>

queue_t _sdk_promises;

SDK_EXPORT void _promise_startup() {
  queue_constructor(&_sdk_promises);
  __promise_startup();
}
SDK_EXPORT void _promise_constructor(task_t* this) {
  queue_push(&_sdk_promises, &this->queue);
  ++__sdk_tasks_count;
}
SDK_EXPORT void _promise_deconstructor(task_t* this) {
  _task_deconstructor(this);
}