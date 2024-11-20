#pragma once
#include "math_structs.h"

constexpr int MAX_TRANSFORMS = 1024;

// Structs
struct Transform {
  IVec2 atlasOffset;
  IVec2 spriteSize;
  Vec2 position;
  Vec2 size;
};

enum SpriteID {
  SPRITE_WHITE,
  SPRITE_DICE,

  SPRITE_COUNT
};

struct Sprite {
  IVec2 atlasOffset;
  IVec2 spriteSize;
};

// Functions
inline Sprite get_Sprite(SpriteID id) {
  Sprite sprite{};
  switch (id) {
    case SPRITE_WHITE: 
      sprite.atlasOffset = {0, 0};
      sprite.spriteSize = {1, 1};
      break;
    case SPRITE_DICE: 
      sprite.atlasOffset = {16, 0};
      sprite.spriteSize = {16, 16};
      break;
    default: 
      WARN("Invalid sprite id: " << id);
      break;
  }
  return sprite;
}