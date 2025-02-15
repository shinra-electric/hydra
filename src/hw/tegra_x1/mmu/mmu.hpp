#pragma once

#include "common/common.hpp"

namespace Hydra::HW::MMU {

class Memory;

class MMUBase {
  public:
    void MapMemory(Memory* mem);
    void UnmapMemory(Memory* mem);
    void RemapMemory(Memory* mem);
    virtual void ReprotectMemory(Memory* mem) = 0;

    Memory* UnmapPtrToMemory(uptr addr) const;
    uptr UnmapPtr(uptr addr) const;

    template <typename T> T Load(uptr addr) const {
        return *reinterpret_cast<T*>(UnmapPtr(addr));
    }

    template <typename T> void Store(uptr addr, T value) const {
        *reinterpret_cast<T*>(UnmapPtr(addr)) = value;
    }

  protected:
    virtual void MapMemoryImpl(Memory* mem) = 0;
    virtual void UnmapMemoryImpl(Memory* mem) = 0;

  private:
    std::vector<Memory*> memories;
};

} // namespace Hydra::HW::MMU
