#pragma once

#include "hw/tegra_x1/gpu/engines/engine_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

class Compute : public EngineBase {
  public:
    void Method(u32 method, u32 arg) override;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
