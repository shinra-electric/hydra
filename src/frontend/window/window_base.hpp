#pragma once

#include "common/common.hpp"

namespace Hydra::Frontend::Window {

class WindowBase {
  public:
    virtual ~WindowBase() {}

    virtual void Run() = 0;
};

} // namespace Hydra::Frontend::Window
