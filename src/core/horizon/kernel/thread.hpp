#pragma once

#include "core/horizon/kernel/const.hpp"
#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::Horizon::Kernel {

class Thread : public Handle {
  public:
    Thread(HW::TegraX1::CPU::MemoryBase* tls_mem_, vaddr_t tls_addr_,
           vaddr_t entry_point_, vaddr_t args_addr_, vaddr_t stack_top_addr_,
           i32 priority_)
        : tls_mem{tls_mem_}, tls_addr{tls_addr_}, entry_point{entry_point_},
          args_addr{args_addr_}, stack_top_addr{stack_top_addr_},
          priority{priority_} {}
    ~Thread() override;

    void Start();

  private:
    HW::TegraX1::CPU::MemoryBase* tls_mem;
    vaddr_t tls_addr;
    vaddr_t entry_point;
    vaddr_t args_addr;
    vaddr_t stack_top_addr;
    i32 priority;

    std::thread* t = nullptr;
};

} // namespace Hydra::Horizon::Kernel
