#ifndef PROMISE_H
#define PROMISE_H

#include <sdk/queue.h>
#include <sdk/task.h>
#include <sdk/error.h>

extern queue_t _global_promises; // queue_t<task_t>

export void _promise_constructor(task_t* this);
export void _promise_deconstructor(task_t* this);
export void _promise_post(task_t* this, err_t value);

export void _promise_startup();
export void _promise_shutdown();
export void _promise_run();

export error_t __promise_startup();


#endif