#include <stdio.h>
#include "String_t.h"
#include "Window.h"

int main() {
  printf("start\n");
  Window* window = window_new_centered("test", 800, 600, WINDOW_RESIZABLE);
  while (window_run()) {
  }
  return 0;
}
