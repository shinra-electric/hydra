#pragma once

#include "common/common.hpp"

namespace Hydra::HW::TegraX1::CPU {

class Memory;

class MMUBase {
  public:
    void Map(Memory* mem, uptr base);
    void Unmap(uptr base);
    void Remap(uptr base);
    // virtual void ReprotectMemory(uptr base) = 0;

    Memory* FindMemoryForAddr(uptr addr, uptr& out_base) const;
    uptr UnmapAddr(uptr addr) const;

    template <typename T> T Load(uptr addr) const {
        return *reinterpret_cast<T*>(UnmapAddr(addr));
    }

    template <typename T> void Store(uptr addr, T value) const {
        *reinterpret_cast<T*>(UnmapAddr(addr)) = value;
    }

  protected:
    virtual void MapImpl(Memory* mem, uptr base) = 0;
    virtual void UnmapImpl(Memory* mem, uptr base) = 0;

  private:
    std::map<uptr, Memory*> mapped_ranges;
};

} // namespace Hydra::HW::TegraX1::CPU
