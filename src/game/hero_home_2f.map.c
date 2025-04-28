#include <game/hero_home_2f.map.h>

#define HERO_HOME_2F_WIDTH 11
#define HERO_HOME_2F_HEIGHT 9
#define HERO_HOME_2F_SIZE (HERO_HOME_2F_WIDTH * HERO_HOME_2F_HEIGHT)

export void hero_home_2f_load() {
  const u8 tiles[TILEMAP_LAYERS][HERO_HOME_2F_SIZE] = {
    { // BG0
      1,  2,  2,  3,  4,  2,  2,  2,  2,  2,  5,
      6,  7,  8,  9, 10, 11, 11, 12, 11, 11, 13,
      14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 19,
      19, 23, 23, 23, 23, 24, 23, 25, 26, 27, 23,
      28, 29, 30, 31, 32, 33, 32, 34, 23, 23, 23,
      35, 36, 37, 38, 39, 40, 39, 41, 23, 23, 23,
      42, 43, 44, 38, 39, 45, 39, 41, 23, 23, 23,
      19, 23, 23, 46, 47, 47, 47, 48, 23, 23, 23,
      19, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23
    }
  };
  tilemap.width = HERO_HOME_2F_WIDTH;
  tilemap.height = HERO_HOME_2F_HEIGHT;
  tilemap.size = HERO_HOME_2F_SIZE;
  tilemap_reserve(sizeof(tiles));
  memory_copy(tilemap.tiles, tiles, sizeof(tiles));
  tilemap_set_player(0.f, 0.f);
  window_updated = true;
}