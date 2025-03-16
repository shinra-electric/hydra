#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

enum class LDOperand : u64 {
    Invalid,
    B32,
    B64,
    B96,
    B128,
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::LDOperand,
                       Invalid, "invalid", B32, "b32", B64, "b64", B96, "b96",
                       B128, "b128")
