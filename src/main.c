#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void window_Callback(Window* window) {
  switch (window_event_get(window)) {

  case WINDOW_QUIT:
    printf("WINDOW_QUIT\n");
    break;

  case WINDOW_UNDEFINED:
    printf("WINDOW_UNDEFINED\n");
    break;

  case WINDOW_FOCUS:
    printf("WINDOW_FOCUS\n");
    break;

  case WINDOW_BLUR:
    printf("WINDOW_BLUR\n");
    break;

  case WINDOW_RESIZE:
    printf("WINDOW_RESIZE\n");
    break;

  case WINDOW_MOUSE_MOVE: {
    int xPos = window_mouse_get_x(window);
    int yPos = window_mouse_get_y(window);
    printf("WINDOW_MOUSE_MOVE (%d, %d)\n", xPos, yPos);
  }break;

  }
}

int main() {

  window_new_centered(window_Callback, "test1", 800, 600, WINDOW_RESIZABLE);

  windows_run();

  return 0;
}
