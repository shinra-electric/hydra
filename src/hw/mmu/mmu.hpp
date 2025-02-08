#pragma once

#include "common.hpp"

namespace Hydra::HW::MMU {

class Memory;

class MMUBase {
  public:
    void MapMemory(Memory* mem);
    void UnmapMemory(Memory* mem);
    void RemapMemory(Memory* mem);
    virtual void ReprotectMemory(Memory* mem) = 0;

    Memory* UnmapPtrToMemory(uptr ptr);
    uptr UnmapPtr(uptr ptr);

  protected:
    virtual void MapMemoryImpl(Memory* mem) = 0;
    virtual void UnmapMemoryImpl(Memory* mem) = 0;

  private:
    std::vector<Memory*> memories;
};

} // namespace Hydra::HW::MMU
