#pragma once

#include "core/input/controller.hpp"

namespace hydra::input::apple_gc {

class Controller : public IController {
  public:
    Controller(id handle_) : handle{handle_} {}

  protected:
    bool IsPressedImpl(ControllerInput input) override;
    f32 GetAxisValueImpl(ControllerInput input) override;

  private:
    id handle;
};

} // namespace hydra::input::apple_gc
