#include "internal/handle.h"
#include "internal/queue.h"
#include "internal/application.h"

void handle_init(handle* this, handle_type type) {
  this->type = type;
  if (app_main.app_queue_tail) {
    app_main.app_queue_tail->next = (queue*)this;
    app_main.app_queue_tail = (queue*)this;
  } else {
    app_main.app_queue_head = (queue*)this;
    app_main.app_queue_tail = (queue*)this;
  }
}