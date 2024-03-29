#ifdef SDK_APPLICATION_H

#include <sdk/events.h>
#include <sdk/assert.h>
#include <sdk/leaks.h>

static u64 __application_max_fd = 0;

void app_new(application_t* this) {
  queue_head(&this->__tasks);
}

i32 app_run(application_t* this) {
  assert(this->__tasks.next);
  assert(this->__tasks.prev);
  queue_t* head = &this->__tasks;
  queue_t* it = (queue_t*)this->__tasks.next;
  queue_t* next;
  while (
    !queue_empty(head)
#ifdef SDK_WINDOW_H
    || window_pooling()
#endif
    ) {
    if (it == head) {
      it = head->next;
    } else {
      next = it->next;
      event_listener_t* listener = (event_listener_t*)it;
      listener->callback(listener->context);
      it = next;
    }
  }
#ifdef SDK_NET_H
  net_shutdown();
#endif
  leaks_warning();
  return 0;
}

#endif