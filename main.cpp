#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include "src/MapGenerator.hpp"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static const SDL_DisplayMode *mode = NULL;
static Uint64 startTime = 0;
static double freqInv = 0.0;
std::vector<SDL_Vertex> verts;
Retrograde::MapGenerator mapGenerator;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  srand(time(0));

  SDL_SetAppMetadata("Retrograde", "1.0", "retrograde");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Force VSync and hardware acceleration
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); 
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  mode = SDL_GetCurrentDisplayMode(display);
  if (!mode) {
    SDL_Log("Failed to get current display mode: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Retrograde", mode->w, mode->h, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  //SDL_SetWindowFullscreen(window, true);

  mapGenerator = Retrograde::MapGenerator(mode->w, mode->h);
  mapGenerator.generateMap(200);
  
  startTime = SDL_GetPerformanceCounter();
  freqInv = 1.0 / (double)SDL_GetPerformanceFrequency();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

  if (event->type == SDL_EVENT_KEY_DOWN) {
    SDL_Keycode key = event->key.key;

    if (key == SDLK_ESCAPE) return SDL_APP_SUCCESS;
    if (key == SDLK_SPACE) mapGenerator.generateMap(200);
  }

  return SDL_APP_CONTINUE;
}

void renderStars(double elapsedSeconds) {
  for (int i = 0; i < mapGenerator.starPoints.size(); ++i) {
    float brightness = 255 * (0.7f + 0.3f * SDL_sin(elapsedSeconds + i));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, brightness);
    SDL_RenderPoint(renderer, mapGenerator.starPoints[i].x, mapGenerator.starPoints[i].y);
  }
}

void renderTerrain(double elapsedSeconds) {
  float brightness = 0.4f + 0.1f * SDL_sin(elapsedSeconds);

  for (SDL_Vertex &vert : mapGenerator.triangulatedTerrain) {
    vert.color.r = 0;
    vert.color.g = 0;
    vert.color.b = 0;
  }

  SDL_RenderGeometry(renderer, NULL, mapGenerator.triangulatedTerrain.data(), mapGenerator.triangulatedTerrain.size(), NULL, 0);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for (int i = 1; i < mapGenerator.terrainPoints.size(); i++) {
    SDL_RenderLine(
      renderer,
      mapGenerator.terrainPoints[i - 1].x,
      mapGenerator.terrainPoints[i - 1].y,
      mapGenerator.terrainPoints[i].x,
      mapGenerator.terrainPoints[i].y
    ); 
  }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  Uint64 now = SDL_GetPerformanceCounter();
  double elapsedSeconds = (now - startTime) * freqInv;

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  renderStars(elapsedSeconds);
  renderTerrain(elapsedSeconds);

  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  // SDL cleans up the window and renderer automatically.
}
