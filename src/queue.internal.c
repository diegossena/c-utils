#include "internal/queue.h"
// TODO: remove this line
#include "sdk/console.h"

void queue_push(queue_t** head, queue_t* q) {
  if (*head) {
    q->prev = (*head)->prev;
    q->prev->next = q;
    q->next = 0;
  } else {
    *head = q;
  }
  (*head)->prev = q;
}
void queue_remove(queue_t** head, queue_t* q) {
  if (*head == q) {
    *head = q->next;
    if (*head) {
      (*head)->prev = *head;
    }
  } else {
    q->prev->next = q->next;
    q->next->prev = q->prev;
  }
}