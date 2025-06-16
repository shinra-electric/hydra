#pragma once

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/observer_base.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer {
class GuestShaderState;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp::Analyzer {
class MemoryAnalyzer;
}

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

class BuilderBase : public ObserverBase {
  public:
    BuilderBase(const DecompilerContext& context_,
                const Analyzer::MemoryAnalyzer& memory_analyzer_,
                const GuestShaderState& state_, std::vector<u8>& out_code_,
                ResourceMapping& out_resource_mapping_)
        : context{context_}, memory_analyzer{memory_analyzer_}, state{state_},
          out_code{out_code_}, out_resource_mapping{out_resource_mapping_} {}
    virtual ~BuilderBase() {}

    virtual void InitializeResourceMapping() = 0;
    virtual void Start() = 0;
    virtual void Finish() = 0;

  protected:
    const DecompilerContext& context;
    const Analyzer::MemoryAnalyzer& memory_analyzer;
    const GuestShaderState& state;

    std::vector<u8>& out_code;
    ResourceMapping& out_resource_mapping;
};

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
