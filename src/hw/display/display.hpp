#pragma once

#include "common/common.hpp"

namespace Hydra::HW::Display {

class DisplayBase {
  public:
    virtual void Open() {}  // TODO: = 0
    virtual void Close() {} // TODO: = 0
};

} // namespace Hydra::HW::Display
