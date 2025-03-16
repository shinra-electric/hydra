#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

typedef u8 reg_t;

constexpr reg_t RZ = 255;

enum class DataType {
    Int,
    UInt,
    Float,
};

enum class SVSemantic {
    Invalid,
    Position,
    UserInOut,
    // TODO: more
};

struct SV {
    SVSemantic semantic;
    u8 index;
    u8 component_index;
    // TODO: more?
};

enum class LoadStoreMode {
    Invalid,
    B32,
    B64,
    B96,
    B128,
};

inline u32 GetLoadStoreCount(LoadStoreMode mode) {
    switch (mode) {
    case LoadStoreMode::B32:
        return 1;
    case LoadStoreMode::B64:
        return 2;
    case LoadStoreMode::B96:
        return 3;
    case LoadStoreMode::B128:
        return 4;
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown load store mode {}", mode);
        return 0;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::DataType, Int,
                       "i", UInt, "u", Float, "f")

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::SVSemantic,
                       Invalid, "invalid", Position, "position", UserInOut,
                       "user in out")

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::LoadStoreMode,
                       Invalid, "invalid", B32, "b32", B64, "b64", B96, "b96",
                       B128, "b128")
