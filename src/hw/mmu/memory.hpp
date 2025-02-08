#pragma once

#include "common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::MMU {

class Memory {
  public:
    Memory(uptr base_, usize size_, Horizon::Permission permission_);
    ~Memory();

    void Resize(usize size_);

    void Clear();

    uptr MapPtr(uptr p);

    uptr UnmapPtr(uptr p);

    bool PtrIsInRange(uptr p);

    // Getters
    uptr GetPtr() const { return ptr; }

    u8* GetPtrU8() const { return (u8*)ptr; }

    uptr GetBase() const { return base; }

    usize GetSize() const { return size; }

    Horizon::Permission GetPermission() const { return permission; }

    // Setters
    void SetPermission(Horizon::Permission permission_) {
        permission = permission_;
    }

  private:
    uptr ptr;

    uptr base;
    usize size;

    Horizon::Permission permission;

    void Allocate();
};

} // namespace Hydra::HW::MMU
