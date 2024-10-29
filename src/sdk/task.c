#include <sdk/task.h>

queue_t _global_tasks;
u64 _global_tasks_count = 0;
task_t* __global_task_it = (task_t*)&_global_tasks;

SDK_EXPORT void task_manager_startup() {
  queue_constructor(&_global_tasks);
#ifdef SDK_NET_H
  __net_startup();
#endif
#ifdef SDK_PROMISE_H
  _promise_startup();
#endif
}
SDK_EXPORT void task_manager_run() {
  task_t* task, * task_next = (task_t*)_global_tasks.next;
  while (_global_tasks_count) {
    task = task_next;
    task_next = (task_t*)task->queue.next;
    if ((queue_t*)task != &_global_tasks) {
      task->callback(task->context, 0, 0);
    }
#ifdef SDK_PROMISE_H
    _promise_run();
#endif
  }
#ifdef SDK_NET_H
  __net_shutdown();
#endif
#ifdef SDK_DEVELOPMENT
  if (__leaks_count) {
    console_color(ANSI_FORE_RED);
    console_log("leak %d", __leaks_count);
    console_color(ANSI_RESET);
  }
#endif
}

// TASK

SDK_EXPORT void _task_constructor(task_t* this) {
  queue_push(&_global_tasks, &this->queue);
  ++_global_tasks_count;
}
SDK_EXPORT void _task_deconstructor(task_t* this) {
  queue_remove(&this->queue);
  --_global_tasks_count;
}
SDK_EXPORT void _task_call_destroy(task_t* this) {
  this->destroy(this->context);
}