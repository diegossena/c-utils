#ifndef SDK_WINDOW_GFX_COLOR_H
#define SDK_WINDOW_GFX_COLOR_H

#define COLOR_WHITE (color_t){ 1.f, 1.f, 1.f, 1.f }
#define COLOR_BLACK (color_t){ 0.f, 0.f, 0.f, 1.f }
#define COLOR_RED (color_t){ 1.f, 0.f, 0.f, 1.f }
#define COLOR_GREEN (color_t){ 0.f, 1.f, 0.f, 1.f }
#define COLOR_BLUE (color_t){ 0.f, 0.f, 1.f, 1.f }
#define COLOR_CYAN (color_t){ 0.f, 1.f, 1.f, 1.f }
#define COLOR_YELLOW (color_t){ 1.f, 1.f, 0.f, 1.f }
#define COLOR_ORANGE (color_t){ 1.f, .4f, 0.f, 1.f }
#define COLOR_DARKBLUE (color_t){ .0f, .0f, .55f, 1.f }

typedef struct gfx_color_t gfx_color_t;

typedef struct color_t { f32 r, g, b, a; } color_t;

// void gfx_color_new(gfx_color_t*, window_t*, color_t);
// void gfx_color_free(gfx_color_t*);

#endif