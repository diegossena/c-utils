#include <sdk/queue.h>

void queue_push(queue_t* head, queue_t* this) {
  this->next = head;
  this->prev = head->prev;
  this->prev->next = this;
  head->prev = this;
}
void queue_remove(queue_t* this) {
  this->prev->next = this->next;
  this->next->prev = this->prev;
}