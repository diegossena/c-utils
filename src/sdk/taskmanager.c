#include <sdk/taskmanager.h>

SDK_EXPORT void _task_constructor(task_t* this, taskmanager_t* taskmanager) {
  this->taskmanager = taskmanager;
  queue_push(&taskmanager->tasks, &this->queue);
  ++taskmanager->tasks_count;
}
SDK_EXPORT void _task_pending(task_t* this) {
  queue_remove(&this->queue);
  queue_push(&this->taskmanager->tasks_pending, &this->queue);
}
SDK_EXPORT void _task_handle(task_t* this) {
  queue_remove(&this->queue);
  queue_push(&this->taskmanager->tasks, &this->queue);
}
SDK_EXPORT void _task_deconstructor(task_t* this) {
  queue_remove(&this->queue);
  --this->taskmanager->tasks_count;
}
SDK_EXPORT void _task_call_destroy(task_t* this) {
  this->destroy(this->context);
}

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->tasks);
  queue_constructor(&this->tasks_pending);
  this->tasks_count = 0;
#ifdef SDK_NET_H
  __net_startup();
#endif
  __taskmanager_constructor_platform(this);
}
SDK_EXPORT void taskmanager_run(taskmanager_t* this) {
  u64 i;
  task_t* task, * task_next = (task_t*)this->tasks.next;

  while (this->tasks_count) {
    task = task_next;
    task_next = (task_t*)task->queue.next;
    if ((queue_t*)task != &this->tasks) {
      task->handle(task->context);
    }
    __taskmanager_pool(this);
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
  __taskmanager_deconstructor_platform(this);
}