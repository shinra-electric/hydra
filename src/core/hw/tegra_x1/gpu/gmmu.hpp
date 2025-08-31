#pragma once

#include "core/hw/generic_mmu.hpp"
#include "core/hw/tegra_x1/gpu/const.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMmu;
}

namespace hydra::hw::tegra_x1::gpu {

struct AddressSpace {
    uptr ptr;
    usize size;
};

// TODO: free memory
class GMmu : public GenericMmu<GMmu, AddressSpace> {
  public:
    usize ImplGetSize(const AddressSpace& as) const { return as.size; }

    AddressSpace& UnmapAddrToAddressSpace(uptr gpu_addr) {
        uptr base;
        auto addr_space = FindAddrImplRef(gpu_addr, base);
        ASSERT_DEBUG(addr_space, Gpu,
                     "Address space not found for Gpu address 0x{:x}",
                     gpu_addr);

        return *addr_space;
    }

    uptr UnmapAddr(uptr gpu_addr);

    void MapImpl(uptr base, AddressSpace as) {}
    void UnmapImpl(uptr base, AddressSpace as) {}

    // Address space
    uptr CreateAddressSpace(uptr ptr, usize size, uptr gpu_addr) {
        if (ptr == 0x0)
            ptr = reinterpret_cast<uptr>(malloc(size));

        AddressSpace as;
        as.ptr = ptr;
        as.size = size;

        if (gpu_addr == invalid<uptr>()) {
            gpu_addr = address_space_base;
            address_space_base += align(size, GPU_PAGE_SIZE);
        }
        Map(gpu_addr, as);

        return gpu_addr;
    }

    uptr AllocatePrivateAddressSpace(usize size, uptr gpu_addr) {
        return CreateAddressSpace(0x0, size, gpu_addr);
    }

    uptr MapBufferToAddressSpace(uptr ptr, usize size, uptr gpu_addr) {
        return CreateAddressSpace(ptr, size, gpu_addr);
    }

    // TODO: correct?
    void ModifyAddressSpace(uptr ptr, usize size, uptr gpu_addr) {
        auto& as = UnmapAddrToAddressSpace(gpu_addr);
        ASSERT_DEBUG(size == as.size, Gpu, "Size mismatch: {} != {}", size,
                     as.size)

        as.ptr = ptr;
    }

  private:
    // TODO: use a better way to allocate new memory
    uptr address_space_base{GPU_PAGE_SIZE};
};

} // namespace hydra::hw::tegra_x1::gpu
