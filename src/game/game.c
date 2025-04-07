#include <game/game.h>

game_t state;

export void game_load() {
  state.repaint = true;
  titlescreen_load();
}