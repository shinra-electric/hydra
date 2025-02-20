#pragma once

#include "hw/display/display_base.hpp"
#include "hw/display/sdl3/const.hpp"

namespace Hydra::HW::Display::SDL3 {

class Display : public DisplayBase {
  public:
    Display();
    ~Display() override;

    void Open() override {}
    void Close() override {}

    void Run() override;

  protected:
    LayerBase* CreateLayerImpl(u32 binder_id) override;

  private:
    SDL_Window* window;
};

} // namespace Hydra::HW::Display::SDL3
