#include <stdio.h>
#include "String_t.h"
#include "Window.h"

int main() {
  Window* window = window_new();

  window_show(window);

  while (window_run()) {
    printf("width %d\n", window->width);
  }
  return 0;
}
