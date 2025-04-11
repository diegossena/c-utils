#include <game/game.h>
#include <sdk/window.h>

void rect_fill_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  f32 r, f32 g, f32 b, f32 a
) {
  // vertex
  u32 vertex_offset = vertices_length;
  vertex_t vertex = {
    .color = { r, g, b, a }
  };
  vertex.x = x0;
  vertex.y = y0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertices_virtual[vertices_length++] = vertex;
  // indexes
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 1;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset + 3;
}
void tile_rect_draw(
  f32 x0, f32 y0, f32 x1, f32 y1,
  u8 tile_x, u8 tile_y
) {
  // tile
  const f32 image_ngc_per_px = (1.f / 160.f) * 16.f;
  const f32 u0 = image_ngc_per_px * tile_x;
  const f32 v0 = image_ngc_per_px * tile_y;
  const f32 u1 = u0 + image_ngc_per_px;
  const f32 v1 = v0 + image_ngc_per_px;
  // vertex
  u32 vertex_offset = vertices_length;
  vertex_t vertex;
  vertex.x = x0;
  vertex.y = y0;
  vertex.uv[0] = u0;
  vertex.uv[1] = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y0;
  vertex.uv[0] = u1;
  vertex.uv[1] = v0;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x1;
  vertex.y = y1;
  vertex.uv[0] = u1;
  vertex.uv[1] = v1;
  vertices_virtual[vertices_length++] = vertex;
  vertex.x = x0;
  vertex.y = y1;
  vertex.uv[0] = u0;
  vertex.uv[1] = v1;
  vertices_virtual[vertices_length++] = vertex;
  // indexes
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 1;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset;
  indexes_virtual[indexes_length++] = vertex_offset + 2;
  indexes_virtual[indexes_length++] = vertex_offset + 3;
}
void text_draw(const char* text, f32 size, f32 x, f32 y) {
  const f32 spacing = size * ndc_per_px_x * .31f;
  while (*text != '\0') {
    u8 tile_x = 0, tile_y = 0;
    if (*text >= 'A' && *text <= 'B') {
      tile_x = *text - 57;
      tile_y = 4;
    } else if (*text >= 'C' && *text <= 'L') {
      tile_x = *text - 'C';
      tile_y = 5;
    } else if (*text >= 'M' && *text <= 'V') {
      tile_x = *text - 'M';
      tile_y = 6;
    } else if (*text >= 'W' && *text <= 'Z') {
      tile_x = *text - 'W';
      tile_y = 7;
    } else {
      switch (*text) {
        case '.':
          tile_x = 4;
          tile_y = 7;
          break;
        case ',':
          tile_x = 5;
          tile_y = 7;
          break;
      }
    }
    if (*text != ' ') {
      const f32 x1 = x + ndc_per_px_x * size;
      const f32 y1 = y - ndc_per_px_y * size;
      tile_rect_draw(x, y, x1, y1, tile_x, tile_y);
      x += spacing;
    } else {
      x += spacing * .5f;
    }
    ++text;
  }
}