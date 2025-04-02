#include <sdk/console.h>
#include <sdk/queue.h>
#include <sdk/async.h>
#include <sdk/taskmanager.h>
#include <sdk/unity.h>

void task_handle(task_t* this) {
  console_log("task_handle %llu", this->context);
  this->destroy(this);
}
i32 main() {
  async_startup();
  taskmanager_startup();

  task_t tasks[2048];
  u64 tasks_length = sizeof(tasks) / sizeof(task_t);
  for (u64 i = 0; i < tasks_length; i++) {
    task_t* task = &tasks[i];
    task->handle = (function_t)task_handle;
    task->destroy = (function_t)task_destroy;
    task->context = (void*)i;
    task_init(task);
  }

  taskmanager_wait();
  async_shutdown();
  return 0;
}