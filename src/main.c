#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void windowCallback(Window* window) {
  switch (window_event_get(window)) {

  case WINDOW_MOUSE_MOVE: {
    int xPos = window_mouse_get_x(window);
    int yPos = window_mouse_get_y(window);
    printf("WINDOW_MOUSE_MOVE (%d, %d)\n", xPos, yPos);
  }break;

  }
}

int main() {

  window_new_centered(windowCallback, "test1", 800, 600, WINDOW_RESIZABLE);

  windows_run();

  return 0;
}
