#ifndef SDK_QUEUE_H
#define SDK_QUEUE_H

#include <sdk/types.h>

#define queue_foreach(type, head) \
  for ( \
    type* it = (type*)head.next, *it_next = (type*)it->queue.next; \
    it != (type*)&head; \
    it = it_next, it_next = (type*)it->queue.next \
  )

typedef struct queue_t queue_t;
typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

#endif