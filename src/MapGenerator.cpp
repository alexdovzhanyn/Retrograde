#include "MapGenerator.hpp"
#include "CartesianPoint.hpp"
#include "Triangulation.hpp"
#include "SDL3/SDL_rect.h"
#include <cmath>
#include <cstdlib>
#include <random>
#include <unordered_map>
#include <iostream>

void Retrograde::MapGenerator::generateMap(int pointDensity, GameLevel &level) {
  generateTerrainVertices(pointDensity, level);
  generateSkyboxStars(level);
}

void Retrograde::MapGenerator::generateTerrainVertices(int pointDensity, GameLevel &level) {
  std::vector<CartesianPoint> &points = level.terrainPoints; 

  points = { CartesianPoint(0, 0), CartesianPoint(screenWidth, 0) };

  float minPointSpacing = (screenWidth / (pointDensity * 2));

  std::vector<float> potentialPointsX;
  for (float i = minPointSpacing; i < screenWidth; i += minPointSpacing) {
    potentialPointsX.push_back(i); 
  }

  // Randomly spread the points on the horizontal axis to get a nice distribution
  std::vector<float> chosen;
  std::sample(
    potentialPointsX.begin(),
    potentialPointsX.end(),
    std::back_inserter(chosen),
    pointDensity,
    std::mt19937{ std::random_device{}() }
  );

  for (float x : chosen) points.push_back(CartesianPoint(x, 0 ));

  std::sort(points.begin(), points.end(), [](CartesianPoint p1, CartesianPoint p2) {
    return p1.x < p2.x;
  });

  // Initialize a random y within our bounds
  float prevY = minAltitude + (rand() % (int)(maxAltitude - minAltitude)); 

  // Add the heights in, but only allow slight variations in altitudes between points so
  // things don't get too spiky
  for (int i = 0; i < points.size(); ++i) {
    int maxAltitudeDeviation = 60;
  
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

  points.push_back(CartesianPoint(screenWidth, 0));
  points.push_back(CartesianPoint(0, 0));

  level.triangulatedTerrain = Triangulation::earClipping(CartesianPoint::toSDLFPoints(points)); 
}

void Retrograde::MapGenerator::generateSkyboxStars(GameLevel &level) {
  int NUM_STARS = 300;
  level.skyboxStarPoints = {};

  for (int i = 0; i < NUM_STARS; i++) {
    level.skyboxStarPoints.push_back(CartesianPoint(
      rand() % (int)screenWidth,
      rand() % (int)screenHeight
    )); 
  }
}

float Retrograde::MapGenerator::clampToAltitudeBounds(float x) {
  float strength = 0.05;
  if (x > maxAltitude) return maxAltitude + std::atan(maxAltitude - x) / strength;
  if (x < minAltitude) return minAltitude - std::atan(x - minAltitude) / strength;
  return x;
}
