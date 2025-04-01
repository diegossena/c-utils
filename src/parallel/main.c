#include <sdk/console.h>
#include <sdk/queue.h>
#include <sdk/async.h>
#include <sdk/taskmanager.h>
#include <sdk/unity.h>

void task1_handle(task_t* this) {
  console_log("task1_handle");
  this->destroy(this);
}
void task2_handle(task_t* this) {
  console_log("task2_handle");
  this->destroy(this);
}

i32 main() {
  async_startup();
  taskmanager_startup();

  task_t task1 = {
    .handle = (function_t)task1_handle,
    .destroy = (function_t)task_deconstructor
  };
  task_constructor(&task1);
  task_t task2 = {
    .handle = (function_t)task2_handle,
    .destroy = (function_t)task_deconstructor
  };
  task_constructor(&task2);

  taskmanager_await();
  taskmanager_shutdown();
  async_shutdown();
  return 0;
}