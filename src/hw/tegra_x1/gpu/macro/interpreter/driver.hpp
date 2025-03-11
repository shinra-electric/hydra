#pragma once

#include "hw/tegra_x1/gpu/macro/driver_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter {

class Driver : public DriverBase {
  public:
  protected:
    void ExecuteImpl(u32 pc) override;
};

} // namespace Hydra::HW::TegraX1::GPU::Macro::Interpreter
