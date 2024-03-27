#ifdef SDK_QUEUE_H

#include <sdk/leaks.h>
#include <sdk/assert.h>

void queue_head(queue_t* this) {
  this->next = this;
  this->prev = this;
}
void queue_forward(queue_t* q) {
  q->prev->next = q->next;
  q->next = q->next->next;
  q->prev->next->next = q;
  q->prev->next->prev = q->prev;
  q->prev = q->prev->next;
}
void queue_backward(queue_t* q) {
  q->next->prev = q->prev;
  q->prev = q->prev->prev;
  q->next->prev->prev = q;
  q->next->prev->next = q->next;
  q->next = q->next->prev;
}
void queue_push(queue_t* head, queue_t* this) {
  this->next = head;
  this->prev = head->prev;
  this->prev->next = this;
  head->prev = this;
  __leaks_memory_increment();
}
void queue_remove(queue_t* this) {
  assert(this->prev);
  assert(this->next);
  this->prev->next = this->next;
  this->next->prev = this->prev;
  __leaks_memory_decrement();
}
void queue_unshift(queue_t* head, queue_t* q) {
  q->next = head->next;
  q->prev = head;
  q->next->prev = q;
  head->next = q;
  __leaks_memory_increment();
}
bool queue_empty(queue_t* head) {
  return head == head->next;
}

#endif