#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void window_callback(Window* window) {
  switch (window_event_get(window)) {
    // Window
    case WINDOW_CLOSE:
      // printf("WINDOW_CLOSE\n");
      break;
    case WINDOW_QUIT:
      // printf("WINDOW_QUIT\n");
      break;
    case WINDOW_UNDEFINED:
      // printf("WINDOW_UNDEFINED\n");
      break;
    case WINDOW_FOCUS:
      // printf("WINDOW_FOCUS\n");
      break;
    case WINDOW_BLUR:
      // printf("WINDOW_BLUR\n");
      break;
    case WINDOW_RESIZE:
      // printf("WINDOW_RESIZE\n");
      break;
      // Mouse
    case MOUSE_MOVE: {
      // int xPos = window_mouse_get_x(window);
      // int yPos = window_mouse_get_y(window);
      // printf("WINDOW_MOUSE_MOVE (%d, %d)\n", xPos, yPos);
    } break;
    case MOUSE_LEFT_DOWN:
      // printf("MOUSE_LEFT_DOWN\n");
      break;
    case MOUSE_LEFT_UP:
      // printf("MOUSE_LEFT_UP\n");
      break;
    case MOUSE_LEFT_DBLCLK:
      // printf("MOUSE_LEFT_DBLCLK\n");
      break;
    case MOUSE_RIGHT_DOWN:
      // printf("MOUSE_RIGHT_DOWN\n");
      break;
    case MOUSE_RIGHT_UP:
      // printf("MOUSE_RIGHT_UP\n");
      break;
    case MOUSE_RIGHT_DBLCLK:
      // printf("MOUSE_RIGHT_DBLCLK\n");
      break;
    case MOUSE_MIDDLE_DOWN:
      // printf("MOUSE_MIDDLE_DOWN\n");
      break;
    case MOUSE_MIDDLE_UP:
      // printf("MOUSE_MIDDLE_UP\n");
      break;
    case MOUSE_MIDDLE_DBLCLK:
      // printf("MOUSE_MIDDLE_DBLCLK\n");
      break;
      // Keyboard
    case KEY_DOWN: {
      Keycode key = window_keypressed(window);
      printf("KEY_DOWN %x\n", key);
    } break;
    case KEY_UP:
      // printf("KEY_UP\n");
      break;
    default:
      printf("%x\n", window_event_get(window));
      break;
  }
}

int main() {

  window_new_centered(window_callback, "test1", 800, 600, WINDOW_RESIZABLE);

  windows_run();

  return 0;
}
