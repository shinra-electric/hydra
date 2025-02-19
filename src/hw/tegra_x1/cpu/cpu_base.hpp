#pragma once

#include "common/common.hpp"
#include "hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::HW::TegraX1::CPU {

class CPUBase {
  public:
    virtual ThreadBase* CreateThread() = 0;
};

} // namespace Hydra::HW::TegraX1::CPU
