#include "CartesianPoint.hpp"
#include "GameManager.hpp"
#include "SDL3/SDL_rect.h"

Retrograde::CartesianPoint::operator SDL_FPoint() const {
  float screenHeight = GameManager::getScreenHeight();

  return SDL_FPoint{ x, screenHeight - y };
}

std::vector<SDL_FPoint> Retrograde::CartesianPoint::toSDLFPoints(const std::vector<Retrograde::CartesianPoint> &points) {
  std::vector<SDL_FPoint> sdlPoints;
  
  for (auto point : points) sdlPoints.push_back(point);

  return sdlPoints;
}
