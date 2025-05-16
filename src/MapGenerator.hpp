#include "SDL3/SDL_render.h"
#include <vector>

namespace Retrograde {
  class MapGenerator {
  public:
    std::vector<SDL_FPoint> terrainPoints;
    std::vector<SDL_Vertex> triangulatedTerrain;
    std::vector<SDL_FPoint> starPoints;

    MapGenerator() = default;
    MapGenerator(float width, float height) : screenWidth(width), screenHeight(height) {
      minAltitude = height - (height / 100.0 * 5);
      maxAltitude = height - (height / 100.0 * 50);
    }

    void generateMap(int pointDensity);

  private:
    float screenWidth;
    float screenHeight;
    float minAltitude;
    float maxAltitude;

    void generateTerrainVertices(int pointDensity);
    void generateSkyboxStars();

    float clampToAltitudeBounds(float x);
  };
}
