#include "Entity.hpp"
#include <iostream>

void Retrograde::Entity::setPosition(float x, float y) {
  float deltaX = x - position.x;
  float deltaY = y - position.y;

  position.x = x;
  position.y = y;
  
  SDL_FPoint screenPosition = position;

  textureRect.x = screenPosition.x - textureWidth / 2.0f;
  textureRect.y = screenPosition.y - textureHeight / 2.0f;

  for (CartesianPoint &point : localCollider) {
    point.x += deltaX;
    point.y += deltaY;
  }
}
