#include <sdk/taskmanager.h>

u64 taskmanager_count = 0;
worker_t __workers[WORKERS_COUNT];
u8 __worker_index = -1;

export void taskmanager_startup() {
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    thread_mutex_startup(&worker->mutex);
    queue_constructor(&worker->tasks);
    worker->thread = thread_new((function_t)__worker_thread, (void*)worker);
  }
}
export void taskmanager_shutdown() {
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    thread_free(worker->thread);
  }
}
export void task_constructor(task_t* this) {
  assert(__worker_index != -1);
  __worker_index = (__worker_index + 1) % WORKERS_COUNT;
  worker_t* worker = &__workers[__worker_index];
  thread_mutex_lock(&worker->mutex);
  queue_push(&worker->tasks, &this->__queue);
  thread_mutex_unlock(&worker->mutex);
  ++taskmanager_count;
}
export void task_deconstructor(task_t* this) {
  queue_remove(&this->__queue);
  --taskmanager_count;
}
export void __worker_thread(worker_t* this) {
  task_t* it = this->tasks.next;
  task_t* next;
  while (true) {
    thread_mutex_lock(&this->mutex);
    next = it->__queue.next;
    thread_mutex_unlock(&this->mutex);
    if (it != (task_t*)&this->tasks) {
      it->handle(it);
    }
    it = next;
  }
}