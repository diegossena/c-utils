#include <sdk/taskmanager.h>
#include <pthread.h>

sync_t* __taskmanager_onexit = 0;
u64 taskmanager_count = 0;
worker_t __workers[WORKERS_COUNT];
u8 __worker_index = -1;

export void taskmanager_startup() {
  assert(__taskmanager_onexit == 0);
  __taskmanager_onexit = sync_new();
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    mutex_init(&worker->lock);
    queue_constructor(&worker->tasks);
    worker->exiting = false;
    worker->sleep = sync_new();
    worker->thread = thread_new((function_t)__worker_thread, (void*)worker);
  }
}
export void taskmanager_wait() {
  assert(__taskmanager_onexit != 0);
  if (taskmanager_count != 0) {
    sync_wait(__taskmanager_onexit);
  }
  thread_t* workers_threads[WORKERS_COUNT];
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    worker->exiting = true;
    sync_signal(worker->sleep);
    workers_threads[i] = worker->thread;
  }
  thread_wait_all(workers_threads, WORKERS_COUNT);
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &__workers[i];
    sync_free(worker->sleep);
    mutex_destroy(&worker->lock);
  }
#ifdef DEBUG
  __taskmanager_onexit = 0;
#endif
}
export void task_init(task_t* this) {
  assert(__worker_index != -1);
  __worker_index = (__worker_index + 1) % WORKERS_COUNT;
  worker_t* worker = &__workers[__worker_index];
  mutex_lock(&worker->lock);
  queue_push(&worker->tasks, &this->__queue);
  mutex_unlock(&worker->lock);
  sync_signal(worker->sleep);
  ++taskmanager_count;
}
export void task_destroy(task_t* this) {
  queue_remove(&this->__queue);
  --taskmanager_count;
  if (taskmanager_count == 0) {
    sync_signal(__taskmanager_onexit);
  }
}
export void __worker_thread(worker_t* this) {
  while (this->exiting == false) {
    sync_wait(this->sleep);
    task_t* it = (task_t*)this->tasks.next;
    task_t* next;
    while (this->tasks.next != &this->tasks) {
      mutex_lock(&this->lock);
      next = (task_t*)it->__queue.next;
      mutex_unlock(&this->lock);
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