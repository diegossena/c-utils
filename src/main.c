#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void window_callback(Window* window) {
  WindowEvent event = window_event_get(window);
  switch (event) {
    // Window
    case WINDOW_LOAD:
      break;
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
    case WINDOW_KEY_DOWN: {
      Keycode key = window_keycode(window);

      switch (key) {
        case KEY_LEFT: printf("KEY_LEFT\n"); break;
        case KEY_UP: printf("KEY_UP\n"); break;
        case KEY_RIGHT: printf("KEY_RIGHT\n"); break;
        case KEY_DOWN: printf("KEY_DOWN\n"); break;
      }
    } break;
    case WINDOW_KEY_UP:
      // printf("KEY_UP\n");
      break;
    default:
      printf("%x\n", event);
      break;
  }
}

int main() {

  window_new(
    window_callback, "test1",
    0, 100,
    800, 600,
    WINDOW_RESIZABLE
  );

  windows_run();

  return 0;
}
