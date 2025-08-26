#pragma once

#include "core/input/device_base.hpp"

namespace hydra::input {

class CursorBase : public DeviceBase {
  public:
    bool ActsAsTouchScreen() const override { return true; }
};

} // namespace hydra::input
