#include "internal/app.task.h"
#include "internal/queue.h"
#include "internal/application.h"

void app_task_register(app_task_t* this, app_task_type type) {
  this->type = type;
  if (app_global.app_queue_tail) {
    app_global.app_queue_tail->next = (queue_t*)this;
    app_global.app_queue_tail = (queue_t*)this;
  } else {
    app_global.app_queue_head = (queue_t*)this;
    app_global.app_queue_tail = (queue_t*)this;
  }
}