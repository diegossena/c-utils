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
  thread_mutex_t mutex;
  thread_cond_t* cond;
  bool exiting;
} worker_t;

typedef struct task_t {
  queue_t __queue;
  function_t handle;
  function_t destroy;
  void* context;
} task_t;

extern thread_cond_t* __taskmanager_onexit;
extern u64 taskmanager_count;
extern worker_t __workers[WORKERS_COUNT];
extern u8 __worker_index;

export void taskmanager_startup();
export void taskmanager_shutdown();
export void taskmanager_await();

export void task_constructor(task_t* this);
export void task_deconstructor(task_t* task);

export void __worker_thread(worker_t* this);

#endif