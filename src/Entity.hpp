#pragma once

#include "CartesianPoint.hpp"
#include "TextureManager.hpp"
#include <vector>
#include <string>

namespace Retrograde {
  class Entity {
  public:
    CartesianPoint position = CartesianPoint(0, 0);
    float rotation = 0.0;

    SDL_FRect textureRect;
    SDL_Texture* texture;
    int textureWidth = 0;
    int textureHeight = 0;

    std::vector<CartesianPoint> localCollider;

    Entity(float x, float y, std::string textureName, int width, int height, bool generateSimpleCollider = false) {
      position.x = x;
      position.y = y;

      texture = TextureManager::getOrLoadTexture(textureName);
      textureWidth = width;
      textureHeight = height;
      textureRect = {
        position.x - textureWidth / 2.0f,
        position.y - textureHeight / 2.0f,
        (float)textureWidth,
        (float)textureHeight
      };

      if (!generateSimpleCollider) return;

      // Simple box collider
      localCollider = {
        CartesianPoint(position.x - textureWidth / 2.0f, position.y - textureHeight / 2.0f),
        CartesianPoint(position.x + textureWidth / 2.0f, position.y - textureHeight / 2.0f),
        CartesianPoint(position.x + textureWidth / 2.0f, position.y + textureHeight / 2.0f),
        CartesianPoint(position.x - textureWidth / 2.0f, position.y + textureHeight / 2.0f)
      }; 
    } 

    void setPosition(float x, float y);
  };
}
