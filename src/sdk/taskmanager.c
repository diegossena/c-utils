#include <sdk/taskmanager.h>
#include <pthread.h>

event_t* __taskmanager_onexit = 0;
u64 taskmanager_count = 0;
worker_t __workers[WORKERS_COUNT];
u8 __worker_index = -1;

export void taskmanager_startup() {
  assert(__taskmanager_onexit == 0);
  __taskmanager_onexit = event_new();
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    mutex_init(&worker->mutex);
    queue_constructor(&worker->tasks);
    worker->exiting = false;
    worker->cond = event_new();
    worker->thread = thread_new((function_t)__worker_thread, (void*)worker);
  }
}
export void taskmanager_await() {
  assert(__taskmanager_onexit != 0);
  event_wait(__taskmanager_onexit);
}
export void taskmanager_shutdown() {
  assert(__taskmanager_onexit != 0);
  thread_t* workers_threads[WORKERS_COUNT];
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    worker->exiting = true;
    event_signal(worker->cond);
    workers_threads[i] = worker->thread;
  }
  thread_wait_all(workers_threads, WORKERS_COUNT);
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    thread_free(worker->thread);
    event_free(worker->cond);
    mutex_destroy(&worker->mutex);
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
  mutex_unlock(&worker->mutex);
  event_signal(worker->cond);
  ++taskmanager_count;
}
export void task_destroy(task_t* this) {
  queue_remove(&this->__queue);
  --taskmanager_count;
  if (taskmanager_count == 0) {
    event_signal(__taskmanager_onexit);
  }
}
export void __worker_thread(worker_t* this) {
  while (this->exiting == false) {
    event_wait(this->cond);
    task_t* it = (task_t*)this->tasks.next;
    task_t* next;
    while (this->tasks.next != &this->tasks) {
      mutex_lock(&this->mutex);
      next = (task_t*)it->__queue.next;
      mutex_unlock(&this->mutex);
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