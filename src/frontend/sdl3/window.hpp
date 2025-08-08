#pragma once

#include "core/emulation_context.hpp"
#include "core/horizon/ui/handler_base.hpp"
#include "frontend/sdl3/cursor.hpp"

// TODO: if macOS
#include "frontend/native/cocoa/native.hpp"
using Native = hydra::frontend::native::cocoa::Native;

namespace hydra::frontend::sdl3 {

class Window : public horizon::ui::HandlerBase {
  public:
    Window(int argc, const char* argv[]);
    ~Window();

    void Run();

    // UI
    void ShowMessageDialog(const horizon::ui::MessageDialogType type,
                           const std::string& title,
                           const std::string& message) override;
    horizon::applets::software_keyboard::SoftwareKeyboardResult
    ShowSoftwareKeyboard(const std::string& header_text,
                         std::string& out_text) override;

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    Native native;
    Cursor cursor;

    EmulationContext emulation_context;
    u64 title_id;

    void BeginEmulation(const std::string& path);

    void UpdateWindowTitle();
    void SetWindowTitle(const std::string& title);
};

} // namespace hydra::frontend::sdl3
