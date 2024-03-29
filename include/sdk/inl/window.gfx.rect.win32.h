#ifdef SDK_WINDOW_GFX_RECT_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

void gfx_rect_draw(const gfx_rect_t* this) {
  ID2D1RenderTarget* render_target = this->window->__d2d_render_target;
  ID2D1Brush* brush = (ID2D1Brush*)this->color->__brush;
  if (this->border_radius) {
    D2D1_ROUNDED_RECT rect = {
      {
        this->rect.left_top.x, this->rect.left_top.y,
        this->rect.right_bottom.x, this->rect.right_bottom.y
      },
      this->border_radius, this->border_radius
    };
    if (this->border_width >= 0) {
      ID2D1RenderTarget_DrawRoundedRectangle(
        render_target, (D2D1_ROUNDED_RECT*)&rect, brush, this->border_width,
        this->stroke->stroke
      );
    } else {
      ID2D1RenderTarget_FillRoundedRectangle(
        render_target, (D2D1_ROUNDED_RECT*)&rect, brush
      );
    }
  } else if (this->border_width > 0) {
    ID2D1RenderTarget_DrawRectangle(
      render_target, (D2D1_RECT_F*)&this->rect, brush, this->border_width,
      this->stroke->stroke
    );
  } else {
    ID2D1RenderTarget_FillRectangle(
      render_target, (D2D1_RECT_F*)&this->rect, brush
    );
  }
}

#endif
#endif