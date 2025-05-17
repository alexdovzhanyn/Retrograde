#pragma once
#include "GameLevel.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"

namespace Retrograde {
  class GameManager {
  public:
    static GameManager& getInstance();

    static SDL_AppResult init();

    static void setGameWindowInfo(float width, float height);

    static void newLevel();

    static GameLevel& getCurrentLevel();

    static Uint64 getCurrentTick();

    static float getScreenHeight();
    static float getScreenWidth();

    static SDL_Renderer* getRenderer();

  private:
    GameManager();    
    static GameManager *instance;

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    float screenWidth;
    float screenHeight;

    Uint64 startTime;
    GameLevel currentLevel;

    SDL_Renderer* renderer;
    SDL_Window* window;
  };
}
