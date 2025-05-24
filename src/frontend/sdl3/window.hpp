#pragma once

#include "core/emulation_context.hpp"
#include "frontend/sdl3/cursor.hpp"

namespace hydra::frontend::sdl3 {

class Window {
  public:
    Window(int argc, const char* argv[]);
    ~Window();

    void Run();

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    Cursor cursor;

    EmulationContext emulation_context;

    void BeginEmulation(const std::string& rom_filename);

    void UpdateWindowTitle();
    void SetWindowTitle(const std::string& title);
};

} // namespace hydra::frontend::sdl3
