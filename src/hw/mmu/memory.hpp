#pragma once

#include "common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::MMU {

class Memory {
  public:
    Memory(uptr base_, usize size_, Horizon::Permission permission_);
    ~Memory();

    void Clear();

    uptr MapPtr(uptr p);

    uptr UnmapPtr(uptr p);

    // Getters
    uptr GetPtr() { return ptr; }

    u8* GetPtrU8() { return (u8*)ptr; }

    uptr GetBase() { return base; }

    usize GetSize() { return size; }

    Horizon::Permission GetPermission() { return permission; }

    // Setters
    void SetPermission(Horizon::Permission permission_) {
        permission = permission_;
    }

  private:
    uptr ptr;

    uptr base;
    usize size;

    Horizon::Permission permission;
};

} // namespace Hydra::HW::MMU
