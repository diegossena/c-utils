#include <stdio.h>
#include "String_t.h"
#include "Window.h"

int main() {
  printf("start\n");
  Window window = window_new_centered("test1", 800, 600, WINDOW_RESIZABLE);
  for (WindowEvent event = WINDOW_UNDEFINED; event; event = window_event_get()) {
    printf("%x\n", event);
  }
  printf("QUIT");
  return 0;
}
