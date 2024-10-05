
#include <sdk/date.h>

#include <sdk/window.h>
#include <sdk/console.h>
#include <sdk/keyboard.h>
#include <sdk/math.h>
#include <sdk/unity.h>

#define SCREEN_SIZE 600
#define SLOT_SIZE 20
#define BOARD_SIZE (SCREEN_SIZE / SLOT_SIZE)
#define BOARD_AREA (BOARD_SIZE * BOARD_SIZE)

// GAME:STATE

u16 food_location;
u16 snake[BOARD_AREA];
u16 snake_length;
key_code_t snake_direction[2];
u8 direction_i = 0;

u32 updated_at = 0;
bool game_over = false;

// GAME:METHODS
u16 game_score() {
  return snake_length - 3;
}
void food_create() {
  i32 slot = -1;
  do {
    slot = math_random_in_range(0, BOARD_AREA);
    for (u16 i = 0; i < snake_length; i++) {
      if (slot == snake[i]) {
        slot = -1;
        break;
      }
    }
  } while (slot == -1);
  food_location = slot;
}
void game_startup() {
  // GAME:STARTUP
  game_over = false;
  // SNAKE:STARTUP
  snake[0] = 63;
  snake[1] = 62;
  snake[2] = 61;
  snake_length = 3;
  snake_direction[0] = snake_direction[1] = KEY_RIGHT;
  // FOOD:STARTUP
  food_create();
}
void slot_draw(u16 position, f32 r, f32 g, f32 b, f32 a) {
  u8 y = position / BOARD_SIZE;
  u8 x = position - y * BOARD_SIZE;
  f32 top = y * SLOT_SIZE;
  f32 left = x * SLOT_SIZE;
  f32 right = left + SLOT_SIZE;
  f32 bottom = top + SLOT_SIZE;
  window_fill_rectangle(
    left, top, right, bottom,
    r, g, b, a
  );
}
// WINDOW:EVENTS
void window_onrender() {
  window_clear();
  // SNAKE:DRAW
  for (u8 i = 0; i < snake_length; i++) {
    slot_draw(snake[i], 0.f, 1.f, 0.f, 1.f);
  }
  // FRUIT:DRAW
  slot_draw(food_location, 1.f, 0.f, 0.f, 1.f);
}
void window_onupdate(u32 time) {
  if (game_over)
    return;
  if (window_focused()) {
    u32 elapsed = time - updated_at;
    if (elapsed >= 100) {
      snake_direction[0] = snake_direction[1];
      updated_at = time;
      u16 snake_tail = snake[snake_length - 1];
      // BODY UPDATE
      for (u16 i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
      }
      // HEAD UPDATE
      switch (snake_direction[0]) {
        case KEY_DOWN:
          if (snake[0] >= BOARD_AREA - BOARD_SIZE) {
            game_over = true;
            return;
          }
          snake[0] += BOARD_SIZE;
          break;
        case KEY_UP:
          if (snake[0] <= BOARD_SIZE) {
            game_over = true;
            return;
          }
          snake[0] -= BOARD_SIZE;
          break;
        case KEY_LEFT:
          if (snake[0] % BOARD_SIZE == 0) {
            game_over = true;
            return;
          }
          snake[0] -= 1;
          break;
        case KEY_RIGHT:
          if (snake[0] % BOARD_SIZE == BOARD_SIZE - 1) {
            game_over = true;
            return;
          }
          snake[0] += 1;
          break;
        default:
      }
      // FOOD COLLISION
      if (snake[0] == food_location) {
        snake[snake_length++] = snake_tail;
        food_create();
      }
      // BODY COLLISION
      for (u8 i = 1; i < snake_length; i++) {
        if (snake[0] == snake[i]) {
          game_over = true;
          return;
        }
      }
      window_redraw();
    }
  }
}
void window_onkeydown(key_code_t key) {
  switch (snake_direction[0]) {
    case KEY_DOWN:
    case KEY_UP:
      switch (key) {
        case KEY_LEFT:
        case KEY_RIGHT:
          snake_direction[1] = key;
          return;
        default:
      }
    case KEY_LEFT:
    case KEY_RIGHT:
      switch (key) {
        case KEY_DOWN:
        case KEY_UP:
          snake_direction[1] = key;
          return;
        default:
      }
    default:
  }
  switch (key) {
    case KEY_SPACE:
      game_startup();
      break;
    case KEY_ESCAPE:
      window_close();
      break;
    default:
  }
}
void window_onkeyup() {}
void window_onkeypress() {}
// MAIN 305473 bytes
i32 main(i32 argc, char** argv) {
  window_startup("Test", SCREEN_SIZE, SCREEN_SIZE);
  game_startup();
  window_run();
  console_color(ANSI_FORE_LIGHTGREEN);
  console_log("SUCCESS");
  console_color(ANSI_RESET);
  return 0;
}