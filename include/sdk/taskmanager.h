#ifndef SDK_TASKMANAGER_H
#define SDK_TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct taskmanager_t {
  queue_t services; // queue_t<task_t>
  queue_t tasks; // queue_t<task_t>
  u64 tasks_count;
} taskmanager_t;

/**
 * function_t handle
 * function_t destroy
 * ?* context
 */
typedef struct task_t {
  queue_t queue;
  void* context;
  function_t handle;
  function_t destroy;
  taskmanager_t* taskmanager;
} task_t;

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this);
SDK_EXPORT void taskmanager_run(const taskmanager_t* this);

SDK_EXPORT void task_constructor(task_t* this, taskmanager_t*);
SDK_EXPORT void task_deconstructor(task_t* this);

#endif