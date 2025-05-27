#ifndef ASYNC_H
#define ASYNC_H

#include <sdk/types.h>
#include <sdk/thread.h>
#include <sdk/assert.h>

extern void* __async;
extern thread_t* __async_thread;

void async_startup();
void async_shutdown();

void __async_worker();

#endif