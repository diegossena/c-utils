#include <sdk/taskmanager.h>

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->services);
  queue_constructor(&this->tasks);
#ifdef SDK_NET_H
  __net_startup();
#endif
}
SDK_EXPORT void taskmanager_run(taskmanager_t* this) {
  u64 i;
  task_t
    * service, * service_next,
    * task, * task_next;
  while (!queue_is_empty(&this->services) || !queue_is_empty(&this->tasks)) {
    service_next = (task_t*)service->queue.next;
    service->handle(service->context);
    service = service_next;
    /**
     * Process immediate callbacks a small fixed number of times
     * to avoid loop starvation.
     */
    i = 0;
    task = (task_t*)this->tasks.next;
    while ((queue_t*)task != &this->tasks && i < 7) {
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