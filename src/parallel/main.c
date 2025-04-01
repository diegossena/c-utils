#include <sdk/console.h>
#include <sdk/queue.h>
#include <sdk/async.h>
#include <sdk/taskmanager.h>
#include <sdk/unity.h>

i32 main() {
  async_startup();
  taskmanager_startup();

  taskmanager_shutdown();
  async_shutdown();
  return 0;
}