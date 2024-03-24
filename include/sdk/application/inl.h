#include <sdk/application.h>
#include <sdk/memory.h>
#include <sdk/events.h>
#include <sdk/assert.h>
#ifdef SDK_WINDOW_H
#include <sdk/window.h>
#endif

void app_new(application_t* this) {
  queue_head(&this->__tasks);
}

i32 app_run(application_t* this) {
  assert(this->__tasks.next);
  assert(this->__tasks.prev);
  queue_t* head = &this->__tasks;
  event_listener_t* it = (event_listener_t*)this->__tasks.next;
  event_listener_t* next;
  while (
    !queue_empty(head)
#ifdef SDK_WINDOW_H
    || window_pooling()
#endif
    ) {
    // if ((queue_t*)it == head) {
    //   it = (event_listener_t*)head->next;
    // }
    // next = (event_listener_t*)it->queue.next;
    // it->callback(it->context);
    // it = next;
  }

#ifdef SDK_NET_H
  net_shutdown();
#endif
  if (memory_leaks > 0) {
    console_warn("memory_leaks=%d", memory_leaks);
  }
  if (queue_count > 0) {
    console_warn("queue_count=%d", queue_count);
  }
  return 0;
}