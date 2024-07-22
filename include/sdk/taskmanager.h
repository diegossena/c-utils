#ifndef SDK_TASKMANAGER_H
#define SDK_TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct task_t {
  // extends
  queue_t queue;
  // public
  callback_t handle;
  void* context;
  callback_t destroy;
} task_t;

typedef struct taskmanager_t {
  queue_t services; // task_t
  queue_t tasks; // task_t
} taskmanager_t;

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this);
SDK_EXPORT void taskmanager_run(taskmanager_t* this);

#endif