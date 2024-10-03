#include <sdk/taskmanager.h>

queue_t _sdk_tasks;
u64 __sdk_tasks_count = 0;
task_t* __sdk_task_it = (task_t*)&_sdk_tasks;

SDK_EXPORT void taskmanager_startup() {
  queue_constructor(&_sdk_tasks);
#ifdef SDK_NET_H
  __net_startup();
#endif
#ifdef SDK_PROMISE_H
  _promise_startup();
#endif
}
SDK_EXPORT void taskmanager_run() {
  while (__sdk_tasks_count)
    _taskmanager_run();
  _taskmanager_deconstructor();
}

SDK_EXPORT void _taskmanager_run() {
  task_t* task_next = (task_t*)__sdk_task_it->queue.next;
  if ((queue_t*)__sdk_task_it != &_sdk_tasks) {
    __sdk_task_it->callback(__sdk_task_it->context, 0, 0);
  }
  __sdk_task_it = task_next;
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
  queue_push(&_sdk_tasks, &this->queue);
  ++__sdk_tasks_count;
}
SDK_EXPORT void _task_deconstructor(task_t* this) {
  queue_remove(&this->queue);
  --__sdk_tasks_count;
}
SDK_EXPORT void _task_call_destroy(task_t* this) {
  this->destroy(this->context);
}