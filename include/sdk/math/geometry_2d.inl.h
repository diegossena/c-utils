#include <sdk/math/geometry_2d.h>
#include <sdk/math.h>

vector2d_t vector2d_sum(const vector2d_t this, const const vector2d_t right) {
  return (vector2d_t) { this.x + right.x, this.y + right.y };
}
vector2d_t vector2d_mul_scalar(const vector2d_t this, f32 scalar) {
  return (vector2d_t) { this.x* scalar, this.y* scalar };
}
f32 vector2d_dot(const vector2d_t this, const vector2d_t right) {
  return this.x * right.x + this.y * right.y;
}

void rect_update_width(rect_t* this) {
  this->right_bottom.x = this->left_top.x + this->width;
}
void rect_update_height(rect_t* this) {
  this->right_bottom.y = this->left_top.y + this->height;
}
void rect_update_size(rect_t* this) {
  rect_update_width(this);
  rect_update_height(this);
}
bool rect_pointin(const rect_t* this, vector2d_t point) {
  return point.x >= this->left_top.x && point.x <= this->right_bottom.x &&
    point.y >= this->left_top.y && point.y <= this->right_bottom.y;
}

overlap_2d_t overlap_rect_rect(const rect_t* rect1, const rect_t* rect2) {
  overlap_2d_t result;
  if (rect1->right_bottom.x >= rect2->left_top.x
    && rect2->right_bottom.x >= rect1->left_top.x
    && rect1->right_bottom.y >= rect2->left_top.y
    && rect2->right_bottom.y >= rect1->left_top.y) {
    result.distance = MAX_F32;
    f32 distance;
    // (1, 0)
    distance = math_abs(rect2->right_bottom.x - rect1->left_top.x);
    if (distance < result.distance) {
      result.distance = distance;
      result.axis.x = 1;
      result.axis.y = 0;
    }
    // (-1, 0)
    distance = math_abs(rect1->right_bottom.x - rect2->left_top.x);
    if (distance < result.distance) {
      result.distance = distance;
      result.axis.x = -1;
      result.axis.y = 0;
    }
    // (0, 1)
    distance = math_abs(rect2->right_bottom.y - rect1->left_top.y);
    if (distance < result.distance) {
      result.distance = distance;
      result.axis.x = 0;
      result.axis.y = 1;
    }
    // (0, -1)
    distance = math_abs(rect1->right_bottom.y - rect2->left_top.y);
    if (distance < result.distance) {
      result.distance = distance;
      result.axis.x = 0;
      result.axis.y = -1;
    }
  }
  // Se houver sobreposição em ambas as direções, os retângulos estão colidindo
  return result;
}