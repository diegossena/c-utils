#include <sdk/taskmanager.h>
#include <sdk/snmp.h>
#include <sdk/unity.h>

i32 main(i32 argc, char** argv) {
  taskmanager_t taskmanager;
  taskmanager_constructor(&taskmanager);



  taskmanager_run(&taskmanager);
  return 0;
}