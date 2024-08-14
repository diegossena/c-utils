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
SDK_EXPORT void queue_unshift(queue_t* head, queue_t* q) {
  q->next = head->next;
  q->prev = head;
  q->next->prev = q;
  head->next = q;
}
SDK_EXPORT void queue_forward(queue_t* this) {
  this->prev->next = this->next;
  this->next = this->next->next;
  this->prev->next->next = this;
  this->prev->next->prev = this->prev;
  this->prev = this->prev->next;
}
SDK_EXPORT void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
}
SDK_EXPORT bool queue_is_empty(const queue_t* head) { return head == head->next; }