#include "MapGenerator.hpp"
#include "Triangulation.hpp"
#include "SDL3/SDL_rect.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

std::vector<SDL_Vertex> Retrograde::MapGenerator::generateGroundVertices(int pointDensity) {
  std::vector<SDL_FPoint> points = {{ 0, 0 }, { screenWidth, 0 }};
  
  // Randomly spread the points on the horizontal axis to get a nice distribution
  for (int i = 0; i < pointDensity; ++i) {
    points.push_back({
      (float)((rand() % (int)(screenWidth - 100)) + 100),
      0
    });
  }

  std::sort(points.begin(), points.end(), [](SDL_FPoint p1, SDL_FPoint p2) {
    return p1.x < p2.x;
  });

  // Initialize a random y within our bounds
  float prevY = (screenHeight - minAltitude) + (rand() % (int)(maxAltitude - minAltitude)); 

  // Add the heights in, but only allow slight variations in altitudes between points so
  // things don't get too spiky
  for (int i = 0; i < points.size(); ++i) {
    int maxAltitudeDeviation = 80;
  
    int direction = (rand() % 2 == 0) ? 1 : -1;
    if (i > 1) {
      float altitudeDelta = points[i - 2].y - points[i - 1].y;
      direction = (altitudeDelta >= 0) ? -1 : 1; 
    }

    // 80% chance to prefer continuing changing altitude in the direction we're already trending
    if ((rand() % 100) > 80) direction *= -1;

    // Prevent spikes by not allowing 3 consecutive points to go from low -> high -> lower
    if (i > 1 && points[i - 2].y > points[i - 1].y && direction == 1) {
      maxAltitudeDeviation = std::max(1, (int)(points[i - 2].y - points[i - 1].y));
    }

    points[i].y = clampToAltitudeBounds(prevY + ((rand() % maxAltitudeDeviation) * direction));
    
    prevY = points[i].y;
  }

  points.push_back({ screenWidth, screenHeight });
  points.push_back({ 0, screenHeight });

  rawPoints = points;
  triangulatedMap = Triangulation::earClipping(points); 

  return triangulatedMap; 
}

float Retrograde::MapGenerator::clampToAltitudeBounds(float x) {
  float strength = 0.05;
  if (x < maxAltitude) return maxAltitude + std::atan(maxAltitude - x) / strength;
  if (x > minAltitude) return minAltitude + std::atan(x - minAltitude) / strength;
  return x;
}
