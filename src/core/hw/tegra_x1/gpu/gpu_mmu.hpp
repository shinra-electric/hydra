#pragma once

#include "core/hw/generic_mmu.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

struct AddressSpace {
    uptr addr; // CPU address, 0x0 for private memory
    usize size;
};

class GPUMMU : public GenericMMU<GPUMMU, AddressSpace> {
  public:
    GPUMMU(CPU::MMUBase* mmu_) : mmu{mmu_} {}

    usize ImplGetSize(const AddressSpace& as) const { return as.size; }

    AddressSpace& UnmapAddrToAddressSpace(uptr gpu_addr) {
        usize base;
        auto addr_space = FindAddrImplRef(gpu_addr, base);
        ASSERT_DEBUG(addr_space, GPU,
                     "Address space not found for GPU address 0x{:x}",
                     gpu_addr);

        return *addr_space;
    }

    uptr UnmapAddrToCpuAddr(uptr gpu_addr);
    uptr UnmapAddr(uptr gpu_addr);

    void MapImpl(uptr base, AddressSpace as) {}
    void UnmapImpl(uptr base, AddressSpace as) {}

  private:
    CPU::MMUBase* mmu;
};

} // namespace Hydra::HW::TegraX1::GPU
