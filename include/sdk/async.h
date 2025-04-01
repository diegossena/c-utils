#ifndef ASYNC_H
#define ASYNC_H

#include <sdk/types.h>
#include <sdk/thread.h>
#include <sdk/assert.h>

extern void* __async;
extern thread_t* __async_thread;

export void async_startup();
export void async_shutdown();

void __async_worker();

#endif