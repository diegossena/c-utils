#include <sdk/taskmanager.h>

thread_cond_t* __taskmanager_onexit;
u64 taskmanager_count = 0;
worker_t __workers[WORKERS_COUNT];
u8 __worker_index = -1;

export void taskmanager_startup() {
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
  thread_cond_wait(__taskmanager_onexit);
}
export void taskmanager_shutdown() {
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    for (task_t* it = (task_t*)worker->tasks.next;
      (queue_t*)it != &worker->tasks;
      it = (task_t*)it->__queue.next) {
      it->destroy(it);
    }
    worker->exiting = true;
    thread_cond_signal(worker->cond);
    thread_wait(worker->thread);
    thread_free(worker->thread);
    thread_cond_free(worker->cond);
    thread_mutex_destroy(&worker->mutex);
  }
  thread_cond_signal(__taskmanager_onexit);
}
export void task_constructor(task_t* this) {
  assert(__worker_index != -1);
  __worker_index = (__worker_index + 1) % WORKERS_COUNT;
  worker_t* worker = &__workers[__worker_index];
  thread_mutex_lock(&worker->mutex);
  queue_push(&worker->tasks, &this->__queue);
  thread_mutex_unlock(&worker->mutex);
  thread_cond_signal(worker->cond);
  ++taskmanager_count;
}
export void task_deconstructor(task_t* this) {
  queue_remove(&this->__queue);
  --taskmanager_count;
  if (taskmanager_count == 0) {
    taskmanager_shutdown();
  }
}
export void __worker_thread(worker_t* this) {
  while (this->exiting == false) {
    thread_cond_wait(this->cond);
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
}