#include <sdk/console.h>
#include <sdk/queue.h>
#include <sdk/taskmanager.h>
#include <sdk/unity.h>

u64 count = 0;
void task_handle(task_t* task) {
  ++count;
  console_log("task_handle count=%d", count);
  if (count == 16) {
    queue_remove(&task->__queue);
  }
  console_log("!task_handle");
}

i32 main() {
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);

  task_t task = {
    .handle = (function_t)task_handle
  };
  queue_push(&taskmanager.tasks, &task.__queue);

  taskmanager_run(&taskmanager);
  return 0;
}