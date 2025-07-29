#pragma once

#include "common/pool.hpp"

namespace hydra {

// TODO: this needs optimizations real bad
template <typename T>
class DynamicPool : public Pool<DynamicPool<T>, T> {
  public:
    u32 _AllocateIndex() {
        // TODO: look for a free index first

        u32 index = objects.size();
        objects.push_back({});

        return index;
    }

    void _FreeByIndex(u32 index) {
        if (index == objects.size() - 1)
            objects.pop_back();
        else
            free_slots.push_back(index);
    }

    bool _IsValidByIndex(u32 index) const {
        if (index >= objects.size())
            return false;

        return std::find(free_slots.begin(), free_slots.end(), index) ==
               free_slots.end();
    }

    T& _GetByIndex(u32 index) { return objects[index]; }

    const T& _GetByIndex(u32 index) const { return objects[index]; }

    usize GetCapacity() const { return objects.size(); }

  private:
    std::vector<T> objects;
    std::vector<u32> free_slots;
};

} // namespace hydra
