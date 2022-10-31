#include <stdio.h>
#include "String_t.h"
#include "Window.h"

int main() {
  printf("start\n");
  Window* window = window_new(
    "Test",
    WINDOWPOS_CENTERED, WINDOWPOS_CENTERED,
    800, 600,
    WINDOW_FULLSCREEN
  );
  while (window_run()) {
  }
  return 0;
}
