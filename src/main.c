#include <stdio.h>
#include "String_t.h"
#include "Window.h"

void windowCallback(
  Window window,
  UINT message,
  WPARAM wParam,
  LPARAM lParam
) {
  if (message == WM_TIMER)
    return;
  printf("%x\n");
}

int main() {

  window_new_centered(windowCallback, "test1", 800, 600, WINDOW_RESIZABLE);

  windows_run();

  return 0;
}
