#pragma once

#include "common/common.hpp"

namespace Hydra::HW::TegraX1::CPU {

class CPUBase {
  public:
    virtual u64 GetRegX(u8 reg) const = 0;
    virtual void SetRegX(u8 reg, u64 value) = 0;
};

} // namespace Hydra::HW::TegraX1::CPU
