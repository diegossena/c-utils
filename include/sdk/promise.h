#ifndef SDK_PROMISE_H
#define SDK_PROMISE_H

#include <sdk/queue.h>
#include <sdk/taskmanager.h>

extern queue_t _global_promises; // queue_t<task_t>
#ifdef PLATFORM_WINDOWS
extern void* __global_iocp;
#endif

SDK_EXPORT void _promise_constructor(task_t* this);
SDK_EXPORT void _promise_deconstructor(task_t* this);
SDK_EXPORT void _promise_post(task_t* this, i32 value);

SDK_EXPORT void _promise_startup();
SDK_EXPORT void _promise_shutdown();
SDK_EXPORT void _promise_run();

SDK_EXPORT void __promise_startup();


#endif