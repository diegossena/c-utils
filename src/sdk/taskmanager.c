#include <sdk/taskmanager.h>

#define TASKS_LOOP_MAX 7

SDK_EXPORT void task_constructor(task_t* this, taskmanager_t* taskmanager) {
  this->taskmanager = taskmanager;
  queue_push(&taskmanager->tasks, &this->queue);
  ++taskmanager->tasks_count;
}
SDK_EXPORT void task_deconstructor(task_t* this) {
  queue_remove(&this->queue);
  --this->taskmanager->tasks_count;
}
SDK_EXPORT void _task_call_destroy(task_t* this) {
  this->destroy(this->context);
}

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->services);
  queue_constructor(&this->tasks);
  this->tasks_count = 0;
#ifdef SDK_NET_H
  __net_startup();
#endif
}
SDK_EXPORT void taskmanager_run(const taskmanager_t* this) {
  u64 i;
  task_t
    * service = (task_t*)&this->services, * service_next,
    * task = (task_t*)&this->tasks, * task_next;
  bool running = false;
  while (!queue_is_empty(&this->services) || !queue_is_empty(&this->tasks)) {
    service_next = (task_t*)service->queue.next;
    if ((queue_t*)service != &this->services) {
      service->handle(service->context);
    }
    service = service_next;
    /**
     * Process immediate callbacks a small fixed number of times
     * to avoid loop starvation.
     */
    i = 0;
    task = (task_t*)this->tasks.next;
    while ((queue_t*)task != &this->tasks && i < TASKS_LOOP_MAX) {
      task_next = (task_t*)task->queue.next;
      task->handle(task->context);
      task = task_next;
      ++i;
    }
  }
#ifdef SDK_NET_H
  __net_shutdown();
#endif
}