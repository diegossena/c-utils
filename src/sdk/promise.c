#include <sdk/promise.h>

queue_t _global_promises;

export void _promise_startup() {
  queue_constructor(&_global_promises);
  __promise_startup();
}
export void _promise_constructor(task_t* this) {
  queue_push(&_global_promises, &this->queue);
  ++_global_tasks_count;
}
export void _promise_deconstructor(task_t* this) {
  _task_deconstructor(this);
}