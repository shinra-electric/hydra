#pragma once

#include "hw/tegra_x1/gpu/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {
class Analyzer;
}

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class BuilderBase : public ObserverBase {
  public:
    BuilderBase(const Analyzer& analyzer_) : analyzer{analyzer_} {}
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

  protected:
    const Analyzer& analyzer;
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
