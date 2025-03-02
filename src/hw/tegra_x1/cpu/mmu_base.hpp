#pragma once

#include "hw/generic_mmu.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MMUBase : public GenericMMU<MMUBase, Memory*> {
  public:
    // virtual void ReprotectMemory(uptr base) = 0;

    template <typename T> T Load(uptr addr) const {
        return *reinterpret_cast<T*>(UnmapAddr(addr));
    }

    template <typename T> void Store(uptr addr, T value) const {
        *reinterpret_cast<T*>(UnmapAddr(addr)) = value;
    }

    uptr UnmapAddr(uptr addr) const {
        uptr base;
        Memory* mem = FindAddrImpl(addr, base);
        if (mem) {
            return reinterpret_cast<uptr>(mem->GetPtrU8() + (addr - base));
        }

        return 0x0;
    }

    usize ImplGetSize(Memory* mem) const { return mem->GetSize(); }

    virtual void MapImpl(uptr base, Memory* mem) = 0;
    virtual void UnmapImpl(uptr base, Memory* mem) = 0;
};

} // namespace Hydra::HW::TegraX1::CPU
