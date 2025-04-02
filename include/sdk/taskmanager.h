#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/memory.h>
#include <sdk/os.h>
#include <sdk/thread.h>
#include <sdk/assert.h>

#define WORKERS_COUNT 16

typedef struct worker_t {
  queue_t tasks;
  thread_t* thread;
  mutex_t lock;
  sync_t* sync;
  bool exiting;
} worker_t;

typedef struct task_t {
  queue_t _queue;
  worker_t* _worker;
  mutex_t _lock;
  function_t handle;
  function_t destroy;
  void* context;
} task_t;

extern sync_t* global_taskmanager_sync;
extern u64 global_taskmanager_count;
extern worker_t global_workers[WORKERS_COUNT];
extern u8 global_worker_index;

export void taskmanager_startup();
export void taskmanager_wait();

export void task_init(task_t* this);
export void task_destroy(task_t* task);

export void __worker_thread(worker_t* this);

#endif