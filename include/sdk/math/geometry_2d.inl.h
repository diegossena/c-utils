#include <sdk/math/geometry_2d.h>

f32 vector2d_dot(const vector2d_t this, const vector2d_t right) {
  return this.x * right.x + this.y * right.y;
}