#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Services::Fssrv {

constexpr usize MAX_PATH_SIZE = 0x301;

enum class EntryType : u32 {
    Directory,
    File,
};

} // namespace Hydra::Horizon::Services::Fssrv
