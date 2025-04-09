#include <sdk/gfx.stroke.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/window.win32.h>

export gfx_stroke_t* gfx_stroke_new() {
  D2D1_STROKE_STYLE_PROPERTIES stroke_properties;
  stroke_properties.startCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.endCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.dashCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.lineJoin = D2D1_LINE_JOIN_ROUND;
  stroke_properties.miterLimit = 10.f;
  stroke_properties.dashStyle = D2D1_DASH_STYLE_SOLID;
  stroke_properties.dashOffset = 0.f;
  stroke_properties.startCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.endCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.dashCap = D2D1_CAP_STYLE_ROUND;
  stroke_properties.lineJoin = D2D1_LINE_JOIN_ROUND;
  stroke_properties.miterLimit = 10.f;
  stroke_properties.dashStyle = D2D1_DASH_STYLE_SOLID;
  stroke_properties.dashOffset = 0.f;
  ID2D1StrokeStyle* this;
  global_d2d_factory->lpVtbl->CreateStrokeStyle(
    global_d2d_factory, &stroke_properties, 0, 0, (ID2D1StrokeStyle**)&this
  );
  return this;
}
export void gfx_stroke_free(gfx_stroke_t* this) {
  ID2D1StrokeStyle_Release((ID2D1StrokeStyle*)this);
}

#endif