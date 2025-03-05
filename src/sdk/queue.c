#include <sdk/queue.h>

export void queue_constructor(queue_t* this) {
  this->next = this;
  this->prev = this;
}
export void queue_push(queue_t* this, queue_t* queue) {
  queue->next = this;
  queue->prev = this->prev;
  queue->prev->next = queue;
  this->prev = queue;
}
export void queue_unshift(queue_t* head, queue_t* q) {
  q->next = head->next;
  q->prev = head;
  q->next->prev = q;
  head->next = q;
}
export void queue_forward(queue_t* this) {
  this->prev->next = this->next;
  this->next = this->next->next;
  this->prev->next->next = this;
  this->prev->next->prev = this->prev;
  this->prev = this->prev->next;
}
export void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
}