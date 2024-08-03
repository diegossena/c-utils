#include <sdk/queue.h>

SDK_EXPORT void queue_constructor(queue_t* this) {
  this->next = this;
  this->prev = this;
}
SDK_EXPORT void queue_push(queue_t* this, queue_t* queue) {
  queue->next = this;
  queue->prev = this->prev;
  queue->prev->next = queue;
  this->prev = queue;
}
SDK_EXPORT void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
}
SDK_EXPORT bool queue_is_empty(const queue_t* head) { return head == head->next; }