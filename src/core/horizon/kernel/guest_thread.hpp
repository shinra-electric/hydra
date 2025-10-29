#pragma once

#include "core/horizon/kernel/thread.hpp"

namespace hydra::hw::tegra_x1::cpu {
class IMemory;
class IThread;
} // namespace hydra::hw::tegra_x1::cpu

namespace hydra::horizon::kernel {

class GuestThread : public IThread {
  public:
    GuestThread(Process* process, vaddr_t stack_top_addr_, i32 priority,
                const std::string_view debug_name = "Thread");
    ~GuestThread() override;

    void SetEntryPoint(vaddr_t entry_point_) { entry_point = entry_point_; }
    void SetArg(u32 index, u64 value) {
        ASSERT(index < sizeof_array(args), Kernel, "Invalid argument index {}",
               index);
        args[index] = value;
    }

    uptr GetTlsPtr() const override;

  protected:
    void Run() override;

  private:
    hw::tegra_x1::cpu::IMemory* tls_mem;
    vaddr_t tls_addr;
    vaddr_t stack_top_addr;

    vaddr_t entry_point{0};
    u64 args[2] = {0};

    hw::tegra_x1::cpu::IThread* thread{nullptr};

  public:
    GETTER(thread, GetThread);
};

} // namespace hydra::horizon::kernel
