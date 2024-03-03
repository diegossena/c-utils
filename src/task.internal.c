// app_global
#include "internal/application.h"
// task_t, queue_push, task_register, task_unregister
#include "internal/task.h"

#include "sdk/console.h" // TODO: remove this line

void task_register(task_t* this) {
  queue_push(&app_global.tasks, &this->queue);
}
void task_unregister(task_t* this) {
  queue_remove(&app_global.tasks, &this->queue);
}