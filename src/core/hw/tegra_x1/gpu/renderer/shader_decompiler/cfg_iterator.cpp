#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/cfg_iterator.hpp"

namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp {

void CfgIterator::Iterate(ObserverBase* observer) {
    auto it = blocks.begin();
    Jump(observer, it->first);
    while (true) {
        const auto res = ParseNextInstruction(observer);
        if (res.code == ResultCode::Error) {
            LOG_ERROR(ShaderDecompiler, "Error");
            return;
        }

        if (GetPC() >= it->second->code_range.end) {
            it++;
            if (it == blocks.end()) {
                return;
            }
            Jump(observer, it->first);
        }
    }
}

} // namespace hydra::hw::tegra_x1::gpu::renderer::shader_decomp
