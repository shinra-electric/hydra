#pragma once

#include "core/input/const.hpp"

namespace hydra::input {

class DeviceBase {
  public:
    virtual ~DeviceBase() {}

    virtual bool ActsAsNpad() const { return false; };
    virtual bool ActsAsTouchScreen() const { return false; };

    virtual bool IsPressed(code_t code) = 0;
    virtual f32 GetAxisValue(code_t code) = 0;
};

} // namespace hydra::input
