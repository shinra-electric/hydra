#pragma once

#include "hw/generic_mmu.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::HW::TegraX1::CPU {

struct MemoryMapping {
    uptr ptr;
    usize size;
};

class MMUBase : public GenericMMU<MMUBase, MemoryMapping> {
  public:
    // virtual void ReprotectMemory(uptr base) = 0;

    uptr UnmapAddr(vaddr addr) const {
        vaddr base;
        MemoryMapping mem = FindAddrImpl(addr, base);

        return reinterpret_cast<uptr>(mem.ptr + (addr - base));
    }

    usize ImplGetSize(MemoryMapping mem) const { return mem.size; }

    virtual void MapImpl(vaddr base, MemoryMapping mem) = 0;
    virtual void UnmapImpl(vaddr base, MemoryMapping mem) = 0;

    void Map(uptr base, Memory* mem) {
        GenericMMU::Map(base, MemoryMapping{mem->GetPtr(), mem->GetSize()});
    }

    void Unmap(uptr base, Memory* mem) {
        GenericMMU::Map(base, MemoryMapping{mem->GetPtr(), mem->GetSize()});
    }
};

} // namespace Hydra::HW::TegraX1::CPU
