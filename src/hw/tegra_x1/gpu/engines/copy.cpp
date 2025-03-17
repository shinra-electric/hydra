#include "hw/tegra_x1/gpu/engines/copy.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(Copy, 0xc0, 1, LaunchDMA, LaunchDMAData)

void Copy::LaunchDMA(const u32 index, const LaunchDMAData data) {
    LOG_DEBUG(Engines, "DMA copy");
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
