#pragma once

#include <sdk/application.h>

/** @brief Externally-defined function to create a application,
 * provided by the consumer of this library.
 * @returns True on successful creation; otherwise false.
 */
extern bool app_main();

i32 main(i32 argc, char** argv) {
  app_inicialize();
  app_main(argc, argv);
  app_run();
  app_shutdown();
}