#pragma once

#include "common/allocators/dynamic_pool.hpp"
#include "common/common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {
class MMUBase;
}

namespace Hydra::HW::TegraX1::GPU {

struct MemoryMap {
    uptr addr = 0;
    usize size;
    bool write;
    // TODO: alignment
    // TODO: kind
};

class GPU {
  public:
    static GPU& GetInstance();

    GPU(CPU::MMUBase* mmu_);
    ~GPU();

    // Memory map
    u32 CreateMap(usize size) {
        Handle handle = memory_maps.AllocateForIndex();
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle);
        memory_map = {};
        memory_map.size = size;

        return handle;
    }

    void AllocateMap(Handle handle, uptr addr, bool write) {
        MemoryMap& memory_map = memory_maps.GetObjectRef(handle);
        memory_map.addr = addr;
        memory_map.write = write;
    }

    u32 GetMapId(Handle handle) { return handle + 1; }

  private:
    CPU::MMUBase* mmu;

    Allocators::DynamicPool<MemoryMap> memory_maps;

    MemoryMap& GetMemoryMapById(u32 id) {
        return memory_maps.GetObjectRef(id - 1);
    }
};

} // namespace Hydra::HW::TegraX1::GPU
