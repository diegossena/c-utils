#include <sdk/sdk.h>

void window_test() {
  window_opt options = {
    .name = "Window",
    .width = 800,
    .height = 600,
    .x = 0,
    .y = 0
  };
  window_inicialize(&options);
}