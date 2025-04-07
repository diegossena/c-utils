#include <game/scenes/titlescreen.h>

export void titlescreen_load() {
  state.titlescreen.loaded = true;
}
export void titlescreen_unload() {
  state.titlescreen.loaded = false;
}