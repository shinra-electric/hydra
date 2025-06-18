#pragma once

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/synchronization_object.hpp"
#include "core/horizon/kernel/thread.hpp"
#include "core/hw/tegra_x1/cpu/memory_base.hpp"

namespace hydra::horizon::kernel {

constexpr u32 MAX_MAIN_THREAD_ARG_COUNT = 2;

struct ProcessParams {
    vaddr_t entry_point{0x0};
    u64 args[MAX_MAIN_THREAD_ARG_COUNT] = {0x0};
    u8 main_thread_priority{0x2c}; // TODO: default value
    u8 main_thread_core_number;
    u32 main_thread_stack_size{0x4000000}; // TODO: default value
    u32 system_resource_size{0x0};
};

class Process : public SynchronizationObject {
  public:
    Process(const ProcessParams& params,
            const std::string_view debug_name = "Process");
    ~Process() override;

    void Run();

  private:
    HandleWithId<Thread> main_thread;
    hw::tegra_x1::cpu::MemoryBase* stack_mem;

    u32 system_resource_size;

  public:
    GETTER(system_resource_size, GetSystemResourceSize);
};

} // namespace hydra::horizon::kernel
