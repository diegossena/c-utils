#include <sdk/queue.h>

void queue_constructor(queue_t* this) {
  this->next = this;
  this->prev = this;
}
void queue_push(queue_t* this, queue_t* queue) {
  queue->next = this;
  queue->prev = this->prev;
  queue->prev->next = queue;
  this->prev = queue;
}
void queue_unshift(queue_t* head, queue_t* q) {
  q->next = head->next;
  q->prev = head;
  q->next->prev = q;
  head->next = q;
}
void queue_forward(queue_t* this) {
  this->prev->next = this->next;
  this->next = this->next->next;
  this->prev->next->next = this;
  this->prev->next->prev = this->prev;
  this->prev = this->prev->next;
}
void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
}

bool queue_is_empty(queue_t* this) {
  return this == this->next;
}
bool queue_is_not_empty(queue_t* this) {
  return this != this->next;
}