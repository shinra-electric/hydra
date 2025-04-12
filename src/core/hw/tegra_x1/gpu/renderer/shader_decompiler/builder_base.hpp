#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer {
class GuestShaderState;
}

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler {

class Analyzer;

class BuilderBase : public ObserverBase {
  public:
    BuilderBase(const Analyzer& analyzer_, const ShaderType type_,
                const GuestShaderState& state_, std::vector<u8>& out_code_,
                ResourceMapping& out_resource_mapping_)
        : analyzer{analyzer_}, type{type_}, state{state_}, out_code{out_code_},
          out_resource_mapping{out_resource_mapping_} {}
    virtual ~BuilderBase() {}

    virtual void Start() = 0;
    virtual void Finish() = 0;

  protected:
    const Analyzer& analyzer;
    const ShaderType type;
    const GuestShaderState& state;

    std::vector<u8>& out_code;
    ResourceMapping& out_resource_mapping;
};

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler
