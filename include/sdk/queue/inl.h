#include <sdk/queue.h>

static i64 queue_count = 0;

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
  ++queue_count;
}
void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
  --queue_count;
}
void queue_unshift(queue_t* head, queue_t* q) {
  q->next = head->next;
  q->prev = head;
  q->next->prev = q;
  head->next = q;
  ++queue_count;
}
bool queue_empty(queue_t* head) {
  return head == head->next;
}