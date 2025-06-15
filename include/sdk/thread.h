#ifndef THREAD_H
#define THREAD_H

#include <sdk/types.h>

typedef void thread_t;
typedef void sync_t;

#ifdef PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION mutex_t;
#endif

thread_t* thread_new(function_t handle, void* context);
void thread_free(thread_t* this);
void thread_wait(thread_t* this);
void thread_wait_all(thread_t** threads, u64 count);

void mutex_init(mutex_t* this);
void mutex_lock(mutex_t* this);
void mutex_unlock(mutex_t* this);
void mutex_destroy(mutex_t* this);

sync_t* sync_new();
void sync_free(sync_t* this);
void sync_wait(sync_t* this);
void sync_signal(sync_t* this);

#endif