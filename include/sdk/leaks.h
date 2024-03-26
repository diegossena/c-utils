#pragma once

#ifdef SDK_DEVELOPMENT

#include <sdk/memory.h>
#include <sdk/queue.h>

static i32 __leaks_memory = 0;
static i32 __leaks_queue = 0;

#define __leaks_memory_increment() ++__leaks_memory
#define __leaks_memory_decrement() --__leaks_memory

#define __leaks_queue_increment() ++__leaks_queue
#define __leaks_queue_decrement() --__leaks_queue

#define leaks_warning() \
  if (__leaks_memory > 0) { \
    console_warn("leaks_memory=%d", __leaks_memory); \
  } \
  if (__leaks_queue > 0) { \
    console_warn("leaks_queue=%d", __leaks_queue); \
  }

#else

#define __leaks_memory_increment()
#define __leaks_memory_decrement()

#define __leaks_queue_increment()
#define __leaks_queue_decrement()

#define leaks_warning()

#endif