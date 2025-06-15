#include <sdk/events.h>

void callback_emit(queue_t* this) {
  callback_t* it = (callback_t*)this->next;
  while ((queue_t*)it != this) {
    it->callback(it->this);
    it = (callback_t*)it->queue.next;
  }
}
void callback_on(queue_t* this, callback_t* callback) {
  queue_push(this, &callback->queue);
}

void listener_on(queue_t* this, listener_t* listener) {
  queue_push(this, &listener->queue);
}
void listener_prepend(queue_t* this, listener_t* listener) {
  queue_unshift(this, &listener->queue);
}
void listener_forward(listener_t* listener) {
  queue_forward(&listener->queue);
}
void listener_off(listener_t* listener) {
  queue_remove(&listener->queue);
}
void listener_emit(queue_t* this, void* argument) {
  listener_t* it = (listener_t*)this->next;
  while ((queue_t*)it != this) {
    it->callback(argument);
    it = (listener_t*)it->queue.next;;
  }
}