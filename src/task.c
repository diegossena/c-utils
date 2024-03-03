// app_global
#include "internal/application.h"
// task_t, queue_t, queue_remove
#include "internal/task.h"
// memory_free
#include "internal/memory.h"
// TODO: remove this line
#include "sdk/console.h"

void task_destroy(void* this) {
  queue_remove(&app_global.tasks, (queue_t*)this);
  memory_free(this);
}