#pragma once

#include "common.hpp"

namespace Hydra::HW::MMU {

class Memory;

class MMUBase {
  public:
    virtual void MapMemory(Memory* mem);
    virtual void UnmapMemory(Memory* mem);
    virtual void ReprotectMemory(Memory* mem) = 0;

    Memory* UnmapPtrToMemory(uptr ptr);
    uptr UnmapPtr(uptr ptr);

  private:
    std::vector<Memory*> memories;
};

} // namespace Hydra::HW::MMU
