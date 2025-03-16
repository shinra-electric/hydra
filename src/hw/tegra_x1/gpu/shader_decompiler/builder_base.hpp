#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class BuilderBase {
  public:
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

  private:
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
