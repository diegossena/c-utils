#ifndef SDK_TASKMANAGER_H
#define SDK_TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct task_t task_t;
typedef struct task_t {
  queue_t queue;
  void* context;
  callback_t handle;
  callback_t destroy;
} task_t;

typedef struct taskmanager_t {
  queue_t services; // queue_t<task_t>
  queue_t tasks; // queue_t<task_t>
} taskmanager_t;

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this);
SDK_EXPORT void taskmanager_run(const taskmanager_t* this);

#endif