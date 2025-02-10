#pragma once

#include "common/common.hpp"

namespace Hydra::Allocators {

#define FREE_SIZE (size + 7) / 8

template <typename T, u32 size> class StaticPool {
  public:
    StaticPool() = default;
    ~StaticPool() = default;

    u32 AllocateForIndex() {
        if (crnt < size)
            return crnt++;

        for (u32 i = 0; i < size; i++) {
            u8& free_slot = free[i / 8];
            u8 mask = (1 << i % 8);
            if (free_slot & mask) {
                free_slot &= ~mask;
                return i;
            }
        }

        Logging::log(Logging::Level::Error, "Free index not found");

        return UINT32_MAX;
    }

    T& Allocate() { return GetObjectRef(AllocateForIndex()); }

    void Free(u32 index) {
        u8& free_slot = free[index / 8];
        u8 mask = (1 << index % 8);
        free_slot &= ~mask;
    }

    // Getters
    T GetObject(u32 index) const { return objects[index]; }
    T& GetObjectRef(u32 index) { return objects[index]; }

  private:
    T objects[size];
    u8 free[FREE_SIZE] = {UINT8_MAX};
    u32 crnt{0};
};

} // namespace Hydra::Allocators
