#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

enum class VertexAttribSize : u32 {
    // One to four 32-bit components
    _1x32 = 0x12,
    _2x32 = 0x04,
    _3x32 = 0x02,
    _4x32 = 0x01,

    // One to four 16-bit components
    _1x16 = 0x1b,
    _2x16 = 0x0f,
    _3x16 = 0x05,
    _4x16 = 0x03,

    // One to four 8-bit components
    _1x8 = 0x1d,
    _2x8 = 0x18,
    _3x8 = 0x13,
    _4x8 = 0x0a,

    // Misc arrangements
    _10_10_10_2 = 0x30,
    _11_11_10 = 0x31,
};

enum class VertexAttribType : u32 {
    None,
    Snorm,
    Unorm,
    Sint,
    Uint,
    Uscaled,
    Sscaled,
    Float,
};

struct VertexAttribState {
    u32 buffer_id : 5;
    bool is_fixed : 1;
    u32 offset : 14;
    VertexAttribSize size : 6;
    VertexAttribType type : 3;
    bool bgra : 1;
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
