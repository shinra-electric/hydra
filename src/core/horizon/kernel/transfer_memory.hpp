#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

class TransferMemory : public AutoObject {
  public:
    TransferMemory(vaddr_t addr_, u64 size_, MemoryPermission perm_,
                   const std::string_view debug_name = "TransferMemory")
        : AutoObject(debug_name), addr{addr_}, size{size_}, perm{perm_} {}

  private:
    vaddr_t addr;
    u64 size;
    MemoryPermission perm;

  public:
    GETTER(addr, GetAddress);
    GETTER(size, GetSize);
    GETTER(perm, GetPermission);
};

} // namespace hydra::horizon::kernel
