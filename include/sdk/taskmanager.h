#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/memory.h>
#include <sdk/os.h>
#include <sdk/thread.h>

typedef struct task_t {
  // extends
  queue_t __queue;
  // props
  function_t handle;
  function_t destroy;
  void* context;
  bool __busy;
  thread_mutex_t __lock;
} task_t;

typedef struct taskmanager_t {
  queue_t tasks; // task_t
  void** threads;
  u8 threads_count;
} taskmanager_t;

export void taskmanager_constructor(taskmanager_t* this);
export void taskmanager_run(taskmanager_t* this);

export void task_startup(taskmanager_t* this, task_t* task);
export void task_shutdown(task_t* task);

export void __taskmanager_run(queue_t* tasks);
export void __taskmanager_startup(taskmanager_t* this);
export void __taskmanager_shutdown(taskmanager_t* this);

#endif