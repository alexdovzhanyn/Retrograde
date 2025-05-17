#include "GameManager.hpp"
#include "GameLevel.hpp"
#include "MapGenerator.hpp"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"

Retrograde::GameManager* Retrograde::GameManager::instance = nullptr;

SDL_AppResult Retrograde::GameManager::init() {
  if (instance != nullptr) return SDL_APP_CONTINUE;

  instance = new GameManager();
  return SDL_APP_CONTINUE;
}

Retrograde::GameManager& Retrograde::GameManager::getInstance() {
  return *instance;
}

Retrograde::GameManager::GameManager() {
  startTime = SDL_GetPerformanceCounter();

  SDL_SetAppMetadata("Retrograde", "1.0", "Retrograde");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    throw std::runtime_error("SDL_Init failed");
  }

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
  if (!mode) {
    SDL_Log("Failed to get current display mode: %s", SDL_GetError());
    throw std::runtime_error("SDL_GetCurrentDisplayMode failed");
  }

  screenWidth = mode->w;
  screenHeight = mode->h;

  if (!SDL_CreateWindowAndRenderer("Retrograde", mode->w, mode->h, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    throw std::runtime_error("SDL_CreateWindowAndRenderer failed");
  }
}

void Retrograde::GameManager::setGameWindowInfo(float width, float height) {
  GameManager &manager = GameManager::getInstance();

  manager.screenWidth = width;
  manager.screenHeight = height;
}

void Retrograde::GameManager::newLevel() {
  GameManager &manager = GameManager::getInstance();

  MapGenerator mapGenerator = MapGenerator(manager.screenWidth, manager.screenHeight);
  mapGenerator.generateMap(200, manager.currentLevel);
}

Retrograde::GameLevel& Retrograde::GameManager::getCurrentLevel() {
  return getInstance().currentLevel;
}

Uint64 Retrograde::GameManager::getCurrentTick() {
  return SDL_GetPerformanceCounter() - GameManager::getInstance().startTime;
}

float Retrograde::GameManager::getScreenHeight() {
  return getInstance().screenHeight;
}

float Retrograde::GameManager::getScreenWidth() {
  return getInstance().screenWidth;
}

SDL_Renderer* Retrograde::GameManager::getRenderer() {
  return getInstance().renderer;
}
