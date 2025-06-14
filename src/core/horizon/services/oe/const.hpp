#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::services::oe {

enum class CpuBoostMode : u32 {
    Normal = 0,
    FastLoad = 1,
};

} // namespace hydra::horizon::services::oe
