#include <sdk/taskmanager.h>

thread_signal_t* __taskmanager_onexit = 0;
u64 taskmanager_count = 0;
worker_t __workers[WORKERS_COUNT];
u8 __worker_index = -1;

export void taskmanager_startup() {
  assert(__taskmanager_onexit == 0);
  __taskmanager_onexit = thread_cond_new();
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    thread_mutex_init(&worker->mutex);
    queue_constructor(&worker->tasks);
    worker->exiting = false;
    worker->cond = thread_cond_new();
    worker->thread = thread_new((function_t)__worker_thread, (void*)worker);
  }
}
export void taskmanager_await() {
  assert(__taskmanager_onexit != 0);
  thread_signal_await(__taskmanager_onexit);
  taskmanager_shutdown();
}
export void taskmanager_shutdown() {
  assert(__taskmanager_onexit != 0);
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    worker->exiting = true;
    thread_signal_emit(worker->cond);

  }
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    thread_await(worker->thread);
    thread_signal_free(worker->cond);
    thread_mutex_destroy(&worker->mutex);
  }
#ifdef DEBUG
  __taskmanager_onexit = 0;
#endif
}
export void task_init(task_t* this) {
  assert(__worker_index != -1);
  __worker_index = (__worker_index + 1) % WORKERS_COUNT;
  worker_t* worker = &__workers[__worker_index];
  thread_mutex_lock(&worker->mutex);
  queue_push(&worker->tasks, &this->__queue);
  thread_mutex_unlock(&worker->mutex);
  thread_signal_emit(worker->cond);
  ++taskmanager_count;
}
export void task_destroy(task_t* this) {
  queue_remove(&this->__queue);
  --taskmanager_count;
  if (taskmanager_count == 0) {
    thread_signal_emit(__taskmanager_onexit);
  }
}
export void __worker_thread(worker_t* this) {
  while (this->exiting == false) {
    thread_signal_await(this->cond);
    task_t* it = (task_t*)this->tasks.next;
    task_t* next;
    while (this->tasks.next != &this->tasks) {
      thread_mutex_lock(&this->mutex);
      next = (task_t*)it->__queue.next;
      thread_mutex_unlock(&this->mutex);
      if (it != (task_t*)&this->tasks) {
        it->handle(it);
      }
      it = next;
    }
  }
  for (task_t* it = (task_t*)this->tasks.next;
    (queue_t*)it != &this->tasks;
    it = (task_t*)it->__queue.next) {
    it->destroy(it);
  }
}