#pragma once

// TODO: support cross-platform time functions

#include <mach/mach_time.h>

#include "common/types.hpp"

namespace Hydra {

inline u64 get_absolute_time() { return mach_absolute_time(); }

} // namespace Hydra
