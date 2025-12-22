#ifndef THREAD_H
#define THREAD_H

#include <sdk/types.h>

typedef void thread_t;
typedef void sync_t;

#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION mutex_t;
#endif

inline thread_t* thread_new(function_t handle, void* context);
inline void thread_free(thread_t* this);
inline void thread_wait(thread_t* this);
inline void thread_wait_all(thread_t** threads, u64 count);

inline void mutex_init(mutex_t* this);
inline void mutex_lock(mutex_t* this);
inline void mutex_unlock(mutex_t* this);
inline void mutex_destroy(mutex_t* this);

inline sync_t* sync_new();
inline void sync_free(sync_t* this);
inline void sync_wait(sync_t* this);
inline void sync_signal(sync_t* this);

#endif