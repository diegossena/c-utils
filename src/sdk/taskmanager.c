#include <sdk/taskmanager.h>

export void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->tasks);
}

export void __taskmanager_run(queue_t* tasks) {
  // console_log("__taskmanager_run");
  task_t* it = (task_t*)tasks->next;
  task_t* next;
  while (queue_is_not_empty((queue_t*)tasks)) {
    next = (task_t*)it->queue.next;
    if ((queue_t*)it != tasks) {
      console_log("busy %d", it->busy);
      if (!it->busy) {
        it->busy = true;
        it->handle(it->context);
        it->busy = false;
      }
    }
    it = next;
  }
#ifdef SDK_NET_H
  __net_shutdown();
#endif
}
export void taskmanager_run(taskmanager_t* this) {
  __taskmanager_startup(this);
  __taskmanager_run(&this->tasks);
  __taskmanager_shutdown(this);
}