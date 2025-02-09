#pragma once

#include "common.hpp"

namespace Hydra::Allocators {

template <typename T> class DynamicPool {
  public:
    DynamicPool() = default;
    ~DynamicPool() = default;

    u32 AllocateForIndex() {
        u32 index = objects.size();
        objects.push_back(nullptr);

        return index;
    }

    T& Allocate() { return GetObjectRef(AllocateForIndex()); }

    void FreeByIndex(u32 index) {
        if (index == objects.size() - 1)
            objects.pop_back();
        else
            free.push_back(index);
    }

    // Getters
    T GetObject(u32 index) const { return objects[index]; }
    T& GetObjectRef(u32 index) { return objects[index]; }

  private:
    std::vector<T> objects;
    std::vector<u32> free;
};

} // namespace Hydra::Allocators
