#include <sdk/taskmanager.h>

sync_t* global_taskmanager_sync = 0;
mutex_t global_taskmanager_lock;
u64 global_taskmanager_count = 0;
worker_t global_workers[WORKERS_COUNT];

export void taskmanager_startup() {
  assert(global_taskmanager_sync == 0);
  global_taskmanager_sync = sync_new();
  mutex_init(&global_taskmanager_lock);
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &global_workers[i];
    mutex_init(&worker->lock);
    queue_constructor(&worker->tasks);
    worker->exiting = false;
    worker->sync = sync_new();
    worker->thread = thread_new((function_t)__worker_thread, (void*)worker);
  }
}
export void taskmanager_wait() {
  assert(global_taskmanager_sync != 0);
  if (global_taskmanager_count != 0) {
    sync_wait(global_taskmanager_sync);
  }
  thread_t* workers_threads[WORKERS_COUNT];
  for (u8 i = 0; i < WORKERS_COUNT; i++) {
    worker_t* worker = &global_workers[i];
    worker->exiting = true;
    sync_signal(worker->sync);
    workers_threads[i] = worker->thread;
  }
  thread_wait_all(workers_threads, WORKERS_COUNT);
#ifdef DEBUG
  global_taskmanager_sync = 0;
#endif
  mutex_destroy(&global_taskmanager_lock);
}

u8 global_worker_index = 0;
u8 global_worker_counter = 0;
export void task_init(task_t* this) {
  assert(global_worker_index != -1);

  if (global_worker_counter++ == MAX_U8) {
    global_worker_index = (global_worker_index + 1) % WORKERS_COUNT;
  }

  // global_worker_index = (global_worker_index + 1) % WORKERS_COUNT;

  worker_t* worker = &global_workers[global_worker_index];
  this->_worker = worker;
  mutex_lock(&worker->lock);
  queue_push(&worker->tasks, &this->_queue);
  mutex_unlock(&worker->lock);
  mutex_lock(&global_taskmanager_lock);
  ++global_taskmanager_count;
  mutex_unlock(&global_taskmanager_lock);
  sync_signal(worker->sync);
}
export void task_destroy(task_t* this) {
  mutex_lock(&this->_worker->lock);
  queue_remove(&this->_queue);
  mutex_unlock(&this->_worker->lock);
  mutex_lock(&global_taskmanager_lock);
  --global_taskmanager_count;
  mutex_unlock(&global_taskmanager_lock);
  if (global_taskmanager_count == 0) {
    sync_signal(global_taskmanager_sync);
  }
}
export void __worker_thread(worker_t* this) {
  while (this->exiting == false) {
    sync_wait(this->sync);
    task_t* it = (task_t*)this->tasks.next;
    task_t* next;
    while (this->tasks.next != &this->tasks) {
      mutex_lock(&this->lock);
      next = (task_t*)it->_queue.next;
      mutex_unlock(&this->lock);
      if (it != (task_t*)&this->tasks) {
        it->handle(it);
      }
      it = next;
    }
  }
  sync_free(this->sync);
  mutex_destroy(&this->lock);
  for (task_t* it = (task_t*)this->tasks.next;
    (queue_t*)it != &this->tasks;
    it = (task_t*)it->_queue.next) {
    it->destroy(it);
  }
}