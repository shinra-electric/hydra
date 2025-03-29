#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::HW::TegraX1::CPU {

Memory::Memory(usize size_, Horizon::Permission permission_, bool is_kernel_)
    : size{align(size_, PAGE_SIZE)}, permission{permission_},
      is_kernel{is_kernel_} {
    Allocate();
}

Memory::~Memory() { free((void*)ptr); }

void Memory::Resize(usize size_) {
    uptr old_ptr = ptr;
    usize old_size = size;

    size = align(size_, PAGE_SIZE);
    Allocate();
    // TODO: is this necessary?
    memcpy((void*)ptr, (void*)old_ptr, std::min(size, old_size));

    // Cleanup
    free((void*)old_ptr);
}

void Memory::Clear() { memset((void*)ptr, 0, size); }

void Memory::Allocate() {
    posix_memalign((void**)(&ptr), PAGE_SIZE, size);
    if (!ptr) {
        LOG_ERROR(MMU, "Failed to allocate memory");
        return;
    }
}

} // namespace Hydra::HW::TegraX1::CPU
