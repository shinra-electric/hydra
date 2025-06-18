#include "core/horizon/kernel/process.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/thread.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace hydra::horizon::kernel {

Process::Process(const ProcessParams& params, const std::string_view debug_name)
    : SynchronizationObject(false, debug_name),
      main_thread(
          new Thread(STACK_REGION_BASE + params.main_thread_stack_size - 0x10,
                     params.main_thread_priority)),
      system_resource_size{params.system_resource_size} {
    // TODO: add main thread handle
    main_thread.handle->SetEntryPoint(params.entry_point);
    for (u32 i = 0; i < MAX_MAIN_THREAD_ARG_COUNT; i++)
        main_thread.handle->SetArg(i, params.args[i]);

    // Stack memory
    auto& mmu = hw::tegra_x1::cpu::MMUBase::GetInstance();
    stack_mem = mmu.AllocateMemory(params.main_thread_stack_size);
    mmu.Map(STACK_REGION_BASE, stack_mem,
            {MemoryType::Stack, MemoryAttribute::None,
             MemoryPermission::ReadWrite});
}

Process::~Process() {
    hw::tegra_x1::cpu::MMUBase::GetInstance().Unmap(STACK_REGION_BASE,
                                                    stack_mem->GetSize());
    hw::tegra_x1::cpu::MMUBase::GetInstance().FreeMemory(stack_mem);
}

void Process::Run() { main_thread.handle->Run(); }

} // namespace hydra::horizon::kernel
