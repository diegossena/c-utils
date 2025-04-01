#ifndef THREAD_H
#define THREAD_H

#include <sdk/types.h>

typedef void thread_t;

#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION thread_mutex_t;
#endif

export thread_t* thread_new(function_t handle, void* context);
export void thread_free(thread_t* this);

export void thread_mutex_startup(thread_mutex_t* this);
export void thread_mutex_lock(thread_mutex_t* this);
export void thread_mutex_unlock(thread_mutex_t* this);
export void thread_mutex_shutdown(thread_mutex_t* this);

#endif