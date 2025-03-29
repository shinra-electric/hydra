#pragma once

#include "common/common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {

constexpr usize ORIGINAL_PAGE_SIZE = 0x1000; // 4KB
constexpr usize PAGE_SIZE = 0x4000;          // 16KB, cause Apple :)

class Memory {
  public:
    Memory(usize size_, Horizon::Permission permission_,
           bool is_kernel_ = false);
    ~Memory();

    void Resize(usize size_);

    void Clear();

    // Getters
    uptr GetPtr() const { return ptr; }

    u8* GetPtrU8() const { return (u8*)ptr; }

    usize GetSize() const { return size; }

    Horizon::Permission GetPermission() const { return permission; }

    bool IsKernel() const { return is_kernel; }

    // Setters
    void SetPermission(Horizon::Permission permission_) {
        permission = permission_;
    }

  private:
    uptr ptr;

    usize size;

    Horizon::Permission permission;
    bool is_kernel;

    void Allocate();
};

} // namespace Hydra::HW::TegraX1::CPU
