#ifndef TASK_H
#define TASK_H

#include <sdk/types.h>
#include <sdk/queue.h>
#include <sdk/error.h>
#include <sdk/console.h>

typedef void (*task_callback_t)(void* this, error_t, u32 bytes);
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
} task_t;

extern queue_t _global_tasks; // queue_t<task_t>
extern u64 _global_tasks_count;
extern task_t* __global_task_it;

export void task_manager_startup();
export void task_manager_run();

export void _task_constructor(task_t* this);
export void _task_deconstructor(task_t* this);
export void _task_call_destroy(task_t* this);

export void _task_manager_shutdown();
export void _task_manager_run();


#endif