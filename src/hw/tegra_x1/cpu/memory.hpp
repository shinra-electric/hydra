#pragma once

#include "common/common.hpp"
#include "horizon/const.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MemoryAllocator;

class Memory {
  public:
    Memory(MemoryAllocator& allocator_, uptr base_, usize size_,
           Horizon::Permission permission_, bool is_kernel_ = false);
    ~Memory();

    void Resize(usize size_);

    // uptr MapPtr(uptr p);

    uptr UnmapAddr(uptr addr);

    bool AddrIsInRange(uptr addr);

    // TODO: these should be replaced with load/store to ensure proper behavior
    // on chunk boundaries
    uptr GetPtr() const;

    u8* GetPtrU8() const;

    // Getters
    uptr GetPa() const { return pa; }

    uptr GetBase() const { return base; }

    usize GetSize() const { return size; }

    Horizon::Permission GetPermission() const { return permission; }

    bool IsKernel() const { return is_kernel; }

    // Setters
    void SetPermission(Horizon::Permission permission_) {
        permission = permission_;
    }

  private:
    MemoryAllocator& allocator;

    uptr pa;
    uptr base;
    usize size;

    Horizon::Permission permission; // TODO: don't force the same permission for
                                    // the whole memory
    bool is_kernel;
};

} // namespace Hydra::HW::TegraX1::CPU
