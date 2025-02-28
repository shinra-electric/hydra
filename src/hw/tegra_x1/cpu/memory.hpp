#pragma once

#include "common/common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {

class Memory {
  public:
    Memory(uptr base_, usize size_, Horizon::Permission permission_,
           bool is_kernel_ = false);
    ~Memory();

    void Resize(usize size_);

    void Clear();

    uptr MapPtr(uptr p);

    uptr UnmapAddr(uptr p);

    bool AddrIsInRange(uptr p);

    // Getters
    uptr GetPtr() const { return ptr; }

    u8* GetPtrU8() const { return (u8*)ptr; }

    uptr GetBase() const { return base; }

    usize GetSize() const { return size; }

    Horizon::Permission GetPermission() const { return permission; }

    bool IsKernel() const { return is_kernel; }

    // Setters
    void SetPermission(Horizon::Permission permission_) {
        permission = permission_;
    }

  private:
    uptr ptr;

    uptr base;
    usize size;

    Horizon::Permission permission;
    bool is_kernel;

    void Allocate();
};

} // namespace Hydra::HW::TegraX1::CPU
