#include <sdk/taskmanager.h>

queue_t _global_tasks;
u64 __global_tasks_count = 0;
task_t* __global_task_it = (task_t*)&_global_tasks;

SDK_EXPORT void taskmanager_startup() {
  queue_constructor(&_global_tasks);
#ifdef SDK_NET_H
  __net_startup();
#endif
#ifdef SDK_PROMISE_H
  _promise_startup();
#endif
}
SDK_EXPORT void taskmanager_run() {
  while (__global_tasks_count)
    _taskmanager_run();
  _taskmanager_deconstructor();
}

SDK_EXPORT void _taskmanager_run() {
  task_t* task_next = (task_t*)__global_task_it->queue.next;
  if ((queue_t*)__global_task_it != &_global_tasks) {
    __global_task_it->callback(__global_task_it->context, 0, 0);
  }
  __global_task_it = task_next;
#ifdef SDK_PROMISE_H
  _promise_run();
#endif
}
SDK_EXPORT void _taskmanager_deconstructor() {
#ifdef SDK_NET_H
  __net_shutdown();
#endif
#ifdef SDK_PROMISE_H
  _promise_shutdown();
#endif
#ifdef SDK_DEVELOPMENT
  if (__leaks_count) {
    console_color(ANSI_FORE_RED);
    console_log("leak %d", __leaks_count);
    console_color(ANSI_RESET);
  }
#endif
}

SDK_EXPORT void _task_constructor(task_t* this) {
  queue_push(&_global_tasks, &this->queue);
  ++__global_tasks_count;
}
SDK_EXPORT void _task_deconstructor(task_t* this) {
  queue_remove(&this->queue);
  --__global_tasks_count;
}
SDK_EXPORT void _task_call_destroy(task_t* this) {
  this->destroy(this->context);
}