#ifdef SDK_EVENTS_H

void emitter_on(queue_t* emitter, event_listener_t* listener) {
  queue_push(emitter, &listener->queue);
}
void emitter_prepend(queue_t* emitter, event_listener_t* listener) {
  queue_unshift(emitter, &listener->queue);
}
void emitter_forward(event_listener_t* listener) {
  queue_forward(&listener->queue);
}
void emitter_off(event_listener_t* listener) {
  queue_remove(&listener->queue);
}
void emitter_emit(queue_t* emitter) {
  event_listener_t* it = (event_listener_t*)emitter->next;
  while (it != (event_listener_t*)emitter) {
    event_listener_t* next = (event_listener_t*)it->queue.next;
    it->callback(it->context);
    it = next;
  }
}
void callback_emit(callback_t* this) {
  if (this->callback) {
    this->callback(this->context);
  }
}
#endif