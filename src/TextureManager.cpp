#include "TextureManager.hpp"
#include "GameManager.hpp"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"

SDL_Texture* Retrograde::TextureManager::getOrLoadTexture(const std::string &textureName) {
  TextureManager &manager = getInstance();

  if (manager.textureAtlas.find(textureName) != manager.textureAtlas.end()) return manager.textureAtlas[textureName];

  SDL_Surface *surface = SDL_LoadBMP(textureName.c_str());
  if (!surface) {
    SDL_Log("Failed to load BMP: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    return nullptr;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(GameManager::getRenderer(), surface);
  SDL_DestroySurface(surface);
  if (!texture) {
    SDL_Log("Failed to create texture: %s", SDL_GetError());
    return nullptr;
  }

  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
  
  manager.textureAtlas[textureName] = texture;

  return texture;
}

void Retrograde::TextureManager::cleanup() {
  TextureManager &manager = getInstance();

  for (auto it : manager.textureAtlas) {
    SDL_DestroyTexture(it.second);
  }
}
