#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "src/CartesianPoint.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include "src/GameManager.hpp"
#include "src/MapGenerator.hpp"
#include "src/Lander.hpp"

static double freqInv = 0.0;

std::vector<SDL_Vertex> verts;
Retrograde::MapGenerator mapGenerator;
Retrograde::Entity *lander;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  srand(time(0));

  try {
    SDL_AppResult result = Retrograde::GameManager::init();

    if (result != SDL_APP_CONTINUE) return result;
  } catch (const std::exception &e) {
    SDL_Log("Initialization error: %s", e.what());
    return SDL_APP_FAILURE;
  }

  lander = new Retrograde::Lander();
  lander->setPosition(600, 600);
  Retrograde::GameManager::newLevel();

  //SDL_SetWindowFullscreen(window, true);
  
  freqInv = 1.0 / (double)SDL_GetPerformanceFrequency();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

  if (event->type == SDL_EVENT_KEY_DOWN) {
    SDL_Keycode key = event->key.key;

    if (key == SDLK_ESCAPE) return SDL_APP_SUCCESS;
    if (key == SDLK_SPACE) Retrograde::GameManager::newLevel();
  }

  return SDL_APP_CONTINUE;
}

void renderLander() {
  SDL_Renderer *renderer = Retrograde::GameManager::getRenderer();
  SDL_RenderTexture(renderer, lander->texture, NULL, &lander->textureRect);

  // SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  // for (int i = 1; i < lander->localCollider.size(); i++) {
  //   SDL_FPoint prev = lander->localCollider[i - 1];
  //   SDL_FPoint curr = lander->localCollider[i];

  //   SDL_RenderLine(renderer, prev.x, prev.y, curr.x, curr.y); 
  // }
}

void renderStars(double elapsedSeconds) {
  Retrograde::GameLevel &level = Retrograde::GameManager::getCurrentLevel();

  int i = 0;
  for (Retrograde::CartesianPoint &point : level.skyboxStarPoints) {
    point.x += 0.5;
    point.y += 0.15;

    float screenWidth = Retrograde::GameManager::getScreenWidth();
    float screenHeight = Retrograde::GameManager::getScreenHeight();

    if (point.x > screenWidth || point.y < 0) {
      if (rand() % 2 == 0) {
        point.x = rand() % (int)screenWidth;
        point.y = 0;
      } else {
        point.x = 0;
        point.y = rand() % (int)screenHeight;
      }
    }

    SDL_Renderer *renderer = Retrograde::GameManager::getRenderer();
    float brightness = 255 * (0.7f + 0.3f * SDL_sin(elapsedSeconds + i));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, brightness);
    SDL_RenderPoint(renderer, point.x, point.y);
    ++i;
  }
}

void renderTerrain(double elapsedSeconds) {
  Retrograde::GameLevel level = Retrograde::GameManager::getCurrentLevel();
  SDL_Renderer *renderer = Retrograde::GameManager::getRenderer();

  SDL_RenderGeometry(renderer, NULL, level.triangulatedTerrain.data(), level.triangulatedTerrain.size(), NULL, 0);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for (int i = 1; i < level.terrainPoints.size(); i++) {
    SDL_FPoint prev = level.terrainPoints[i - 1];
    SDL_FPoint curr = level.terrainPoints[i];

    SDL_RenderLine(renderer, prev.x, prev.y, curr.x, curr.y); 
  }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  double elapsedSeconds = Retrograde::GameManager::getCurrentTick() * freqInv;

  SDL_Renderer *renderer = Retrograde::GameManager::getRenderer();

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  // Poll keyboard state
  int numKeys;
  const bool *keystate = SDL_GetKeyboardState(&numKeys);

  if (keystate[SDL_SCANCODE_W]) lander->setPosition(lander->position.x, lander->position.y + 5);
  if (keystate[SDL_SCANCODE_A]) lander->setPosition(lander->position.x - 5, lander->position.y);
  if (keystate[SDL_SCANCODE_S]) lander->setPosition(lander->position.x, lander->position.y - 5);
  if (keystate[SDL_SCANCODE_D]) lander->setPosition(lander->position.x + 5, lander->position.y);

  renderStars(elapsedSeconds);
  renderTerrain(elapsedSeconds);
  renderLander();

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  delete lander;
}
