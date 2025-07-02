#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/all_paths_iterator.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/cfg_builder.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

void AllPathsIterator::Iterate(ObserverBase* observer) {
    auto cfg_builder = dynamic_cast<analyzer::CfgBuilder*>(observer);
    if (!cfg_builder) {
        LOG_FATAL(ShaderDecompiler, "Observer must be a CFG builder");
        return;
    }

    bool running = true;

    cfg_builder->SetVisitCallback([&](u32 block) { Jump(observer, block); });
    cfg_builder->SetExitCallback([&]() { running = false; });

    while (running) {
        const auto res = ParseNextInstruction(cfg_builder);
        if (res.code == ResultCode::Error) {
            LOG_ERROR(ShaderDecompiler, "Error");
            return;
        }
    }
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
