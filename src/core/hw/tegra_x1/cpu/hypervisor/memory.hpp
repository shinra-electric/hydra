#pragma once

#include "core/hw/tegra_x1/cpu/hypervisor/const.hpp"
#include "core/hw/tegra_x1/cpu/memory.hpp"

namespace hydra::hw::tegra_x1::cpu::hypervisor {

class Memory : public IMemory {
  public:
    Memory(usize size) : IMemory(align(size, APPLE_PAGE_SIZE)) { Allocate(); }
    ~Memory() override { Free(); }

    uptr GetPtr() const override { return ptr; }

  protected:
    void ResizeImpl() override {
        Free();
        Allocate();
    }

  private:
    uptr ptr;

    // Helpers
    void Allocate() {
        ptr = allocate_vm_memory(GetSize());

        // Map
        // TODO: Why does this fail occasionally?
        HV_ASSERT_SUCCESS(
            hv_vm_map(reinterpret_cast<void*>(ptr), ptr, GetSize(),
                      HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC));
    }

    void Free() {
        // Unmap
        HV_ASSERT_SUCCESS(hv_vm_unmap(ptr, align(GetSize(), APPLE_PAGE_SIZE)));

        free(reinterpret_cast<void*>(ptr));
    }
};

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
