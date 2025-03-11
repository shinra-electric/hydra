#include "hw/tegra_x1/gpu/engines/3d.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(ThreeD)

void ThreeD::Macro(u32 method, u32 arg) {
    LOG_WARNING(GPU, "Macro (method: 0x{:08x})", method);
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
