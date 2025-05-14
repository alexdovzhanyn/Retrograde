#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Uint64 startTime = 0;
static double freqInv = 0.0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("Retrograde", "1.0", "retrograde");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Force VSync and hardware acceleration
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); 
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display);
  if (!mode) {
    SDL_Log("Failed to get current display mode: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Retrograde", mode->w, mode->h, SDL_WINDOW_BORDERLESS, &window, &renderer)) {
    SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  //SDL_SetWindowFullscreen(window, true);

  startTime = SDL_GetPerformanceCounter();
  freqInv = 1.0 / (double)SDL_GetPerformanceFrequency();

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  Uint64 now = SDL_GetPerformanceCounter();
  double elapsedSeconds = (now - startTime) * freqInv;

  float red   = 0.5f + 0.5f * SDL_sin(elapsedSeconds);
  float green = 0.5f + 0.5f * SDL_sin(elapsedSeconds + SDL_PI_D * 2.0 / 3.0);
  float blue  = 0.5f + 0.5f * SDL_sin(elapsedSeconds + SDL_PI_D * 4.0 / 3.0);

  SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  // SDL cleans up the window and renderer automatically.
}
