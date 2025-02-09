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

    T* Allocate() { return GetObject(AllocateForIndex()); }

    void FreeByIndex(u32 index) {
        if (index == objects.size() - 1)
            objects.pop_back();
        else
            free.push_back(index);
    }

    void Free(T* object) {
        auto it = objects.find(object);
        if (it == objects.end()) {
            printf("Could not free %p\n", object);
            return;
        }

        FreeByIndex(it.first);
    }

    // Getters
    T* GetObject(u32 index) const { return objects[index]; }

    // Setters
    void SetObject(u32 index, T* object) { objects[index] = object; }

  private:
    std::vector<T*> objects;
    std::vector<u32> free;
};

} // namespace Hydra::Allocators
