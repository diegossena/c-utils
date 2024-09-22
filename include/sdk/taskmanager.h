#ifndef SDK_TASKMANAGER_H
#define SDK_TASKMANAGER_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/error.h>

#if defined(PLATFORM_WINDOWS) && defined(SDK_NET_H)
#define SDK_IOCP
#endif

typedef struct taskmanager_t {
  queue_t tasks; // queue_t<task_t>
  queue_t promises; // queue_t<task_t>
  u64 tasks_count;
#ifdef SDK_IOCP
  void* __iocp;
#endif
} taskmanager_t;

SDK_EXPORT void taskmanager_constructor(taskmanager_t* this);
SDK_EXPORT void taskmanager_run(taskmanager_t* this);

SDK_EXPORT void __taskmanager_constructor(taskmanager_t* this);
SDK_EXPORT void __taskmanager_deconstructor(taskmanager_t* this);
SDK_EXPORT void __taskmanager_pool(taskmanager_t* this);

// TASK

typedef void (*task_callback_t)(void* this, error_code_t, u32 bytes);
/**
 * task_handle_t handle
 * function_t destroy
 * ?* context
 */
typedef struct task_t {
  queue_t queue;
  void* context;
  task_callback_t callback;
  function_t destroy;
  taskmanager_t* taskmanager;
} task_t;

SDK_EXPORT void _task_constructor(task_t* this, taskmanager_t*, bool promise);
SDK_EXPORT void _task_deconstructor(task_t* this);
SDK_EXPORT void _task_call_destroy(task_t* this);

SDK_EXPORT void _promise_post(task_t* this, i32 value);

#endif