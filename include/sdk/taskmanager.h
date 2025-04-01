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
  queue_t __queue;
  function_t handle;
  function_t destroy;
  void* context;
} task_t;

extern sync_t* __taskmanager_sync;
extern u64 taskmanager_count;
extern worker_t __workers[WORKERS_COUNT];
extern u8 __worker_index;

export void taskmanager_startup();
export void taskmanager_wait();

export void task_init(task_t* this);
export void task_destroy(task_t* task);

export void __worker_thread(worker_t* this);

#endif