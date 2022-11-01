#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void windowCallback(Window* window) {
  switch (window->message) {

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
    int xPos = LOWORD(window->lParam);
    int yPos = HIWORD(window->lParam);
    printf("WINDOW_MOUSE_MOVE (%d, %d)\n", xPos, yPos);
  }break;

  }
}

int main() {

  window_new_centered(windowCallback, "test1", 800, 600, WINDOW_RESIZABLE);

  windows_run();

  return 0;
}
