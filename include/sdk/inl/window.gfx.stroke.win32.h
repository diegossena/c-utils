#ifdef SDK_WINDOW_GFX_STROKE_H
#include <sdk/platform.h>
#ifdef PLATFORM_WINDOWS

#include <sdk/assert.h>

typedef struct gfx_stroke_t {
  ID2D1StrokeStyle* stroke;
} gfx_stroke_t;

void gfx_stroke_free(gfx_stroke_t* this) {
  assert(this->stroke);
  ID2D1StrokeStyle_Release(this->stroke);
  __leaks_memory_decrement();
}
void gfx_stroke_new(gfx_stroke_t* this, gfx_stroke_props_t props) {
  assert(props.window);
  assert(!this->stroke);
  window_t* window = props.window;
  ID2D1Factory* factory = window->__d2d_factory;
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
  ID2D1Factory_CreateStrokeStyle(
    factory, &stroke_properties, null, null,
    (ID2D1StrokeStyle**)&this->stroke
  );
  __leaks_memory_increment();
}

#endif
#endif