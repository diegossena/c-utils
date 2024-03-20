#include <sdk/application.h>
#include <sdk/memory.h>
#include <sdk/events.h>
#ifdef SDK_WINDOW_H
#include <sdk/window.h>
#endif

void app_constructor(application_t* this) {
  queue_head(&this->__tasks);
}

i32 app_run(application_t* this) {
  event_listener_t* head = (event_listener_t*)&this->__tasks;
  event_listener_t* it = (event_listener_t*)this->__tasks.next;
  event_listener_t* next;
#ifdef SDK_WINDOW_H
  i32 i = 1;
  while (i && window_pool()) {
    /**
     * Process immediate callbacks (e.g. write_cb) a small fixed number of
     * times to avoid loop starvation.
     */
    for (i = 0; i < 8; i++) {
      if (it == head) {
        it = (event_listener_t*)this->__tasks.next;
        if (it == head) {
          break;
        }
      }
      next = (event_listener_t*)it->queue.next;
      it->callback(it->context);
      it = next;
    }
  }
#else
  while (true) {
#endif
    if (it == head) {
      it = (event_listener_t*)this->__tasks.next;
      if (it == head) {
        break;
      }
    }
    next = (event_listener_t*)it->queue.next;
    it->callback(it->context);
    it = next;
  }
#ifdef SDK_NET_H
  net_shutdown();
#endif
  if (memory_counter > 0) {
    console_warn("memory_counter=%d", memory_counter);
  }
  return 0;
}