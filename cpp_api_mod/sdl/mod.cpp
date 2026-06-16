#include "carrot_module.h"
#include <SDL2/SDL.h>
#include <stdexcept>
#include <string>

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

struct SDLInitFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "init"; }
  Value call(std::vector<Value>) override {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      throw std::runtime_error(std::string("sdl.init(): ") + SDL_GetError());
    return std::monostate{};
  }
};

struct SDLCreateWindowFn : NinCallable {
  int arity() override { return 3; }
  std::string name() override { return "createWindow"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<std::string>(args[0]))
      throw std::runtime_error("sdl.createWindow(): title must be a string.");
    if (!std::holds_alternative<double>(args[1]) ||
        !std::holds_alternative<double>(args[2]))
      throw std::runtime_error(
          "sdl.createWindow(): width and height must be numbers.");

    std::string title = std::get<std::string>(args[0]);
    int w = (int)std::get<double>(args[1]);
    int h = (int)std::get<double>(args[2]);

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if (!window)
      throw std::runtime_error(std::string("sdl.createWindow(): ") +
                               SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
      throw std::runtime_error(
          std::string("sdl.createWindow(): renderer failed: ") +
          SDL_GetError());

    return std::monostate{};
  }
};

struct SDLClearFn : NinCallable {
  int arity() override { return 3; }
  std::string name() override { return "clear"; }
  Value call(std::vector<Value> args) override {
    if (!renderer)
      throw std::runtime_error(
          "sdl.clear(): no renderer, call createWindow first.");
    SDL_SetRenderDrawColor(renderer, (int)std::get<double>(args[0]),
                           (int)std::get<double>(args[1]),
                           (int)std::get<double>(args[2]), 255);
    SDL_RenderClear(renderer);
    return std::monostate{};
  }
};

struct SDLPresentFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "present"; }
  Value call(std::vector<Value>) override {
    if (!renderer)
      throw std::runtime_error(
          "sdl.present(): no renderer, call createWindow first.");
    SDL_RenderPresent(renderer);
    return std::monostate{};
  }
};

struct SDLPollQuitFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "pollQuit"; }
  Value call(std::vector<Value>) override {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        return true;
    }
    return false;
  }
};

struct SDLDelayFn : NinCallable {
  int arity() override { return 1; }
  std::string name() override { return "delay"; }
  Value call(std::vector<Value> args) override {
    if (!std::holds_alternative<double>(args[0]))
      throw std::runtime_error("sdl.delay(): argument must be a number.");
    SDL_Delay((Uint32)std::get<double>(args[0]));
    return std::monostate{};
  }
};

struct SDLQuitFn : NinCallable {
  int arity() override { return 0; }
  std::string name() override { return "quit"; }
  Value call(std::vector<Value>) override {
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
    }
    if (window) {
      SDL_DestroyWindow(window);
      window = nullptr;
    }
    SDL_Quit();
    return std::monostate{};
  }
};

extern "C" void
carrot_module_init(std::unordered_map<std::string, Value> *out) {
  (*out)["init"] = std::make_shared<SDLInitFn>();
  (*out)["createWindow"] = std::make_shared<SDLCreateWindowFn>();
  (*out)["clear"] = std::make_shared<SDLClearFn>();
  (*out)["present"] = std::make_shared<SDLPresentFn>();
  (*out)["pollQuit"] = std::make_shared<SDLPollQuitFn>();
  (*out)["delay"] = std::make_shared<SDLDelayFn>();
  (*out)["quit"] = std::make_shared<SDLQuitFn>();
}
