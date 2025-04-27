#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/thread.hpp"
#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace Hydra::Horizon::Kernel {

class Process : public Handle {
  public:
    Process();
    ~Process() override;

    void Run();

    // Getter
    HandleWithId<Thread>& GetMainThread() { return main_thread; }

  private:
    HandleWithId<Thread> main_thread;
    HW::TegraX1::CPU::MemoryBase* stack_mem;
};

} // namespace Hydra::Horizon::Kernel
