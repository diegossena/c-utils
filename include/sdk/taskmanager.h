#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/memory.h>

typedef struct task_t {
  // extends
  queue_t queue;
  // props
  void* context;
  function_t handle;
  function_t destroy;
  bool busy;
} task_t;

typedef struct taskmanager_t {
  queue_t tasks; // task_t
  void** threads;
  u8 threads_count;
} taskmanager_t;

export void taskmanager_constructor(taskmanager_t* this);
export void taskmanager_run(taskmanager_t* this);

export void __taskmanager_run(queue_t* tasks);
export void __taskmanager_startup(taskmanager_t* this);
export void __taskmanager_shutdown(taskmanager_t* this);

#endif