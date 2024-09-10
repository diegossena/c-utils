#ifndef SDK_TASKMANAGER_H
#define SDK_TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/error.h>

typedef struct taskmanager_t {
  queue_t tasks; // queue_t<task_t>
  queue_t tasks_pending; // queue_t<task_t>
  u64 tasks_count;
#ifdef PLATFORM_WINDOWS
  void* iocp;
#endif
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
SDK_EXPORT void taskmanager_run(taskmanager_t* this);

SDK_EXPORT void _task_constructor(task_t* this, taskmanager_t*);
SDK_EXPORT void _task_deconstructor(task_t* this);
SDK_EXPORT void _task_pending(task_t* this);
SDK_EXPORT void _task_handle(task_t* this);
SDK_EXPORT void _task_call_destroy(task_t* this);

SDK_EXPORT void __taskmanager_constructor_platform(taskmanager_t* this);
SDK_EXPORT void __taskmanager_deconstructor_platform(taskmanager_t* this);
SDK_EXPORT void __taskmanager_pool(taskmanager_t* this);

#endif