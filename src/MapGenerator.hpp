#pragma once
#include "GameLevel.hpp"

namespace Retrograde {
  class MapGenerator {
  public:
    MapGenerator() = default;
    MapGenerator(float width, float height) : screenWidth(width), screenHeight(height) {
      minAltitude = height / 100.0 * 5;
      maxAltitude = height / 100.0 * 50;
    }

    void generateMap(int pointDensity, GameLevel &level);

  private:
    float screenWidth;
    float screenHeight;
    float minAltitude;
    float maxAltitude;

    void generateTerrainVertices(int pointDensity, GameLevel &level);
    void generateSkyboxStars(GameLevel &level);

    float clampToAltitudeBounds(float x);
  };
}
