#pragma once

#include <Hypervisor/Hypervisor.h>

#include "core/hw/tegra_x1/cpu/const.hpp"

namespace Hydra::HW::TegraX1::CPU::Hypervisor {

constexpr usize APPLE_PAGE_SIZE = 0x4000;

#define HV_ASSERT_SUCCESS(ret) assert((hv_return_t)(ret) == HV_SUCCESS)

inline uptr allocate_vm_memory(usize size) {
    ASSERT_ALIGNMENT(size, APPLE_PAGE_SIZE, Hypervisor, "size")

    void* ptr;
    posix_memalign(&ptr, APPLE_PAGE_SIZE, size);
    if (!ptr) {
        LOG_ERROR(Hypervisor, "Failed to allocate memory");
        return 0x0;
    }

    // Clear the memory
    memset(ptr, 0, size);

    return reinterpret_cast<uptr>(ptr);
}

} // namespace Hydra::HW::TegraX1::CPU::Hypervisor
