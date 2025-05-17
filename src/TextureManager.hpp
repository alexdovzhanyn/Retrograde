#pragma once

#include "SDL3/SDL_render.h"
#include <unordered_map>
#include <string>

namespace Retrograde {
  class TextureManager {
  public:
    std::unordered_map<std::string, SDL_Texture*> textureAtlas;

    static TextureManager& getInstance() {
      static TextureManager instance;
      return instance;
    }  

    static SDL_Texture* getOrLoadTexture(const std::string &textureName);

    static void cleanup();

  private:
    TextureManager() {}

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
  };
}
