#pragma once

#include "CartesianPoint.hpp"
#include "SDL3/SDL_render.h"
#include <vector>

namespace Retrograde {
  struct GameLevel {
    std::vector<SDL_Vertex> triangulatedTerrain;
    std::vector<CartesianPoint> terrainPoints;
    std::vector<CartesianPoint> skyboxStarPoints;
  };
}
