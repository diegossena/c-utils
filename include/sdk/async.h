#ifndef ASYNC_H
#define ASYNC_H

#include <sdk/types.h>
#include <sdk/thread.h>
#include <sdk/assert.h>

extern void* _async;
extern thread_t* _async_thread;

void async_startup();
void async_shutdown();

void _async_worker();

#endif