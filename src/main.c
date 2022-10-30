#include <stdio.h>
#include "String_t.h"
#include "Window.h"

int main() {
  String test = string_new("Hello World");

  Window window = window_new();
  window_show(window);
  while (window_run()) {

  }

  return 0;
}
