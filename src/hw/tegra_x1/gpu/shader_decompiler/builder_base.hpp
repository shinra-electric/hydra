#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {
class Analyzer;
}

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class BuilderBase : public ObserverBase {
  public:
    BuilderBase(const Analyzer& analyzer_, const Renderer::ShaderType type_,
                std::vector<u8>& out_code_)
        : analyzer{analyzer_}, type{type_}, out_code{out_code_} {}
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

  protected:
    const Analyzer& analyzer;
    const Renderer::ShaderType type;

    std::vector<u8>& out_code;
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
