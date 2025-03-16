#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

;

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
