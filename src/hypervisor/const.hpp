#pragma once

#include <Hypervisor/Hypervisor.h>

#include "common/common.hpp"

namespace Hydra::Hypervisor {

#define HYP_ASSERT_SUCCESS(ret) assert((hv_return_t)(ret) == HV_SUCCESS)

} // namespace Hydra::Hypervisor
