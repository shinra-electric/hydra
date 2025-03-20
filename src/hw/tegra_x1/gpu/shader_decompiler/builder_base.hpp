#pragma once

#include "hw/tegra_x1/gpu/renderer/const.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU {
class GuestShaderState;
}

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

class Analyzer;

class BuilderBase : public ObserverBase {
  public:
    BuilderBase(const Analyzer& analyzer_, const Renderer::ShaderType type_,
                const GuestShaderState& state_, std::vector<u8>& out_code_,
                Renderer::ResourceMapping& out_resource_mapping_)
        : analyzer{analyzer_}, type{type_}, state{state_}, out_code{out_code_},
          out_resource_mapping{out_resource_mapping_} {}
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

  protected:
    const Analyzer& analyzer;
    const Renderer::ShaderType type;
    const GuestShaderState& state;

    std::vector<u8>& out_code;
    Renderer::ResourceMapping& out_resource_mapping;
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
