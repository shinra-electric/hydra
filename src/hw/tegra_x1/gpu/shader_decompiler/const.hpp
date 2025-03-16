#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/engines/const.hpp"

#define INST0(value, mask) if ((inst & mask##ull) == value##ull)
#define INST(value, mask) else INST0(value, mask)

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

typedef u8 reg_t;

constexpr reg_t RZ = 255;

enum class DataType {
    None,
    Int,
    UInt,
    Float,
};

inline DataType to_data_type(Engines::VertexAttribType vertex_attrib_type) {
    switch (vertex_attrib_type) {
    case Engines::VertexAttribType::Snorm:
        return DataType::Float;
    case Engines::VertexAttribType::Unorm:
        return DataType::Float;
    case Engines::VertexAttribType::Sint:
        return DataType::Int;
    case Engines::VertexAttribType::Uint:
        return DataType::UInt;
    case Engines::VertexAttribType::Uscaled:
        return DataType::None;
    case Engines::VertexAttribType::Sscaled:
        return DataType::None;
    case Engines::VertexAttribType::Float:
        return DataType::Float;
    default:
        return DataType::None;
    }
}

inline DataType to_data_type(SurfaceFormat surface_format) {
    // TODO: implement
    return DataType::Float;
}

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

    SV(SVSemantic semantic_, u8 index_ = invalid<u8>(),
       u8 component_index_ = invalid<u8>())
        : semantic{semantic_}, index{index_},
          component_index{component_index_} {}
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

inline const SV GetSVFromAddr(u64 addr) {
    ASSERT_ALIGNMENT_DEBUG(addr, 4, ShaderDecompiler, "Address");

    struct SVBase {
        SVSemantic semantic;
        u64 base_addr;
    };

    static constexpr SVBase bases[] = {
        {SVSemantic::UserInOut, 0x80},
        {SVSemantic::Position, 0x70},
    };

    for (const auto& base : bases) {
        if (addr >= base.base_addr) {
            return SV(base.semantic,
                      static_cast<u8>((addr - base.base_addr) >> 4),
                      static_cast<u8>((addr >> 2) & 0x3));
        }
    }

    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "SV address 0x{:02x}", addr);

    return SVSemantic::Invalid;
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::DataType, Int,
                       "int", UInt, "uint", Float, "float")

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::SVSemantic,
                       Invalid, "invalid", Position, "position", UserInOut,
                       "user in out")

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::ShaderDecompiler::LoadStoreMode,
                       Invalid, "invalid", B32, "b32", B64, "b64", B96, "b96",
                       B128, "b128")
