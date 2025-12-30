#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::services::oe {

enum class CpuBoostMode : u32 {
    Normal = 0,
    FastLoad = 1,
};

} // namespace hydra::horizon::services::oe

ENABLE_ENUM_FORMATTING(hydra::horizon::services::oe::CpuBoostMode, Normal,
                       "normal", FastLoad, "fast load")
