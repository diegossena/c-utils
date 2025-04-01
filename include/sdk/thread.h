#ifndef THREAD_H
#define THREAD_H

#include <sdk/types.h>

typedef void thread_t;
typedef void thread_signal_t;

#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION thread_mutex_t;
#endif

export thread_t* thread_new(function_t handle, void* context);
export void thread_await(thread_t* this);

export void thread_mutex_init(thread_mutex_t* this);
export void thread_mutex_lock(thread_mutex_t* this);
export void thread_mutex_unlock(thread_mutex_t* this);
export void thread_mutex_destroy(thread_mutex_t* this);

export thread_signal_t* thread_cond_new();
export void thread_signal_free(thread_signal_t* this);
export void thread_signal_await(thread_signal_t* this);
export void thread_signal_emit(thread_signal_t* this);

#endif