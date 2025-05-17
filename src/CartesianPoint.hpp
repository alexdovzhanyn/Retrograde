#pragma once
#include "SDL3/SDL_rect.h"
#include <vector>

namespace Retrograde {
  class CartesianPoint {
  public:
    float x;
    float y;

    CartesianPoint(float x, float y) : x(x), y(y) {}

    // Converts the point to screen coordinates (flips the y axis)
    operator SDL_FPoint() const;

    static std::vector<SDL_FPoint> toSDLFPoints(const std::vector<CartesianPoint> &points);
  };
}
