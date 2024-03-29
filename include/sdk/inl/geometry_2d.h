#ifdef SDK_MATH_GEOMETRY_2D_H

vector2d_t vector2d_sum(const vector2d_t this, const const vector2d_t right) {
  return (vector2d_t) { this.x + right.x, this.y + right.y };
}
vector2d_t vector2d_mul_scalar(const vector2d_t this, f32 scalar) {
  return (vector2d_t) { this.x* scalar, this.y* scalar };
}
f32 vector2d_dot(const vector2d_t this, const vector2d_t right) {
  return this.x * right.x + this.y * right.y;
}

void rect_set_width(rect_t* this, u16 value) {
  this->right_bottom.x = this->left_top.x + value;
}
void rect_set_height(rect_t* this, u16 value) {
  this->right_bottom.y = this->left_top.y + value;
}
void rect_set_size(rect_t* this, u16 value) {
  rect_set_width(this, value);
  rect_set_height(this, value);
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

#endif