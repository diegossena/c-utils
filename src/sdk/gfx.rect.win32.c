#include <sdk/gfx.rect.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

void gfx_fill_rect(const f32 rect[4], gfx_color_t* color) {
  global_d2d_render_target->lpVtbl->FillRectangle(
    global_d2d_render_target, (D2D1_RECT_F*)rect, (ID2D1Brush*)color
  );
}

void gfx_draw_rect(const f32 rect[4], gfx_color_t* color, f32 width, gfx_stroke_t* stroke) {
  global_d2d_render_target->lpVtbl->DrawRectangle(
    global_d2d_render_target, (D2D1_RECT_F*)rect, (ID2D1Brush*)color, width,
    (ID2D1StrokeStyle*)stroke
  );
}

#endif