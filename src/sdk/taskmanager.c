#include <sdk/taskmanager.h>

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->tasks);
#ifdef SDK_NET_H
  __net_startup();
#endif
}
SDK_EXPORT void taskmanager_run(taskmanager_t* this) {
  u64 i;
  task_t* it, * next;
  while (!queue_is_empty(&this->tasks)) {
    it = (task_t*)this->tasks.next;
    i = 0;
    /**
     * Process immediate callbacks a small fixed number of times
     * to avoid loop starvation.
     */
    while ((queue_t*)it != &this->tasks && i < 8) {
      next = (task_t*)it->queue.next;
      it->handle(it->context);
      it = next;
      ++i;
    }
  }
#ifdef SDK_NET_H
  __net_shutdown();
#endif
}