#pragma once

struct Vec2 {
  float x, y;
  inline Vec2 operator/(float scalar) {
    return Vec2{x / scalar, y / scalar};
  }
  inline Vec2 operator-(Vec2 other) {
    return Vec2{x - other.x, y - other.y};
  }
};
struct IVec2 {
  int x, y;
};
inline Vec2 ivec2_to_vec2(IVec2 v) {
  return Vec2{(float)v.x, (float)v.y};
}

struct Vec4 {
  union {
    float values[4];
    struct {
      float x;
      float y;
      float z;
      float w;
    };
    struct {
      float r, g, b, a;
    };
  };
  float& operator[] (int index) {
    return values[index];
  }
};

struct Matrix4 {
  union {
    Vec4 values[4];
    struct {
      float ax;
      float ay;
      float az;
      float aw;

      float bx;
      float by;
      float bz;
      float bw;

      float cx;
      float cy;
      float cz;
      float cw;

      float dx;
      float dy;
      float dz;
      float dw;
    };
  };
  Vec4& operator[] (int index) {
    return values[index];
  }
};

inline Matrix4 camera_projection(float left, float right, float top, float bottom) {
  // https://learnwebgl.brown37.net/08_projections/projections_ortho.html
  Matrix4 result{};
  result.aw = -(right + left) / (right - left);
  result.bw = (top + bottom) / (top - bottom);
  result.cw = 0.0f; // Near Plane
  result[0][0] = 2.0f / (right - left);
  result[1][1] = 2.0f / (top - bottom);
  result[2][2] = 1.0f / (1.0f - 0.0f); // Far and Near
  result[3][3] = 1.0f;
  return result;
}