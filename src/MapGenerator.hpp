#include "SDL3/SDL_render.h"
#include <vector>

namespace Retrograde {
  class MapGenerator {
  public:
    std::vector<SDL_FPoint> rawPoints;
    std::vector<SDL_Vertex> triangulatedMap;

    MapGenerator() = default;
    MapGenerator(float width, float height) : screenWidth(width), screenHeight(height) {
      minAltitude = height - (height / 100.0 * 10);
      maxAltitude = height - (height / 100.0 * 70);
    }

    std::vector<SDL_Vertex> generateGroundVertices(int pointDensity);

  private:
    float screenWidth;
    float screenHeight;
    float minAltitude;
    float maxAltitude;

    float clampToAltitudeBounds(float x);
  };
}
