#include <sdk/taskmanager.h>

export void taskmanager_constructor(taskmanager_t* this) {
  queue_constructor(&this->tasks);
}
export void task_startup(taskmanager_t* this, task_t* task) {
  task->__busy = false;
  thread_mutex_startup(&task->__lock);
  task_t* tail = this->tasks.prev;
  if (tail != (task_t*)&this->tasks) {
    thread_mutex_lock(tail->__lock);
    queue_push(&this->tasks, &task->__queue);
    thread_mutex_unlock(tail->__lock);
  } else {
    queue_push(&this->tasks, &task->__queue);
  }
}
export void __taskmanager_run(queue_t* tasks) {
  task_t* it = (task_t*)tasks->next;
  task_t* next;
  while (queue_is_not_empty((queue_t*)tasks)) {
    next = (task_t*)it->__queue.next;
    if ((queue_t*)it != tasks) {
      if (!it->__busy) {
        it->__busy = true;
        it->handle(it);
        it->__busy = false;
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

export void __taskmanager_startup(taskmanager_t* this) {
  this->threads_count = os_cpu_count() - 1;
  this->threads = memory_alloc(sizeof(void*) * this->threads_count);
  for (u8 i = 0; i < this->threads_count; i++) {
    this->threads[i] = thread_new(__taskmanager_run, &this->tasks);
  }
}
export void __taskmanager_shutdown(taskmanager_t* this) {
  for (u8 i = 0; i < this->threads_count; i++) {
    thread_free(this->threads[i]);
  }
}