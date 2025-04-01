#ifndef THREAD_H
#define THREAD_H

#include <sdk/types.h>

typedef void thread_t;
typedef void event_t;

#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION mutex_t;
#endif

export thread_t* thread_new(function_t handle, void* context);
export void thread_free(thread_t* this);
export void thread_wait(thread_t* this);
export void thread_wait_all(thread_t** threads, u64 count);
export void thread_wait_once(thread_t** threads, u64 count);

export void mutex_init(mutex_t* this);
export void mutex_lock(mutex_t* this);
export void mutex_unlock(mutex_t* this);
export void mutex_destroy(mutex_t* this);

export event_t* event_new();
export void event_free(event_t* this);
export void event_wait(event_t* this);
export void event_signal(event_t* this);

#endif