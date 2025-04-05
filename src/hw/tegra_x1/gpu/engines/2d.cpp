#include "hw/tegra_x1/gpu/engines/2d.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(TwoD, 0x237, 1, Copy, u32)

void TwoD::Copy(const u32 index, const u32 pixels_from_memory_src_y0_int) {
    LOG_FUNC_STUBBED(Engines);
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
