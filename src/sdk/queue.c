#include <sdk/queue.h>

SDK_EXPORT void queue_constructor(queue_t* this) {
  this->next = this;
  this->prev = this;
}
SDK_EXPORT bool queue_is_empty(queue_t* head) { return head == head->next; }