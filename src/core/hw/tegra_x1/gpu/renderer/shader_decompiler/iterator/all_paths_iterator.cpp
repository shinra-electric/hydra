#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/iterator/all_paths_iterator.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator {

void AllPathsIterator::Iterate(ObserverBase* observer) {
    while (true) {
        const auto res = ParseNextInstruction(observer);
        switch (res.code) {
        case ResultCode::None:
            break;
        case ResultCode::Branch:
            Jump(observer, res.target);
            break;
        case ResultCode::BranchConditional:
            block_queue.push(res.target);
            Jump(observer, GetPC() + 1);
            break;
        case ResultCode::SyncPoint:
            block_queue.push(res.target);
            break;
        case ResultCode::EndBlock: {
            if (block_queue.empty())
                return;

            const auto target = block_queue.front();
            block_queue.pop();
            Jump(observer, target);
            break;
        }
        case ResultCode::Error:
            LOG_ERROR(ShaderDecompiler, "Error");
            return;
        }
    }
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::Iterator
