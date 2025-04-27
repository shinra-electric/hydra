#include "core/horizon/kernel/process.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/thread.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Kernel {

Process::Process()
    : main_thread(new Thread(STACK_REGION_BASE + DEFAULT_STACK_MEM_SIZE,
                             20)) /* TODO: priority */ {
    // Stack memory
    auto& mmu = HW::TegraX1::CPU::MMUBase::GetInstance();
    stack_mem = mmu.AllocateMemory(DEFAULT_STACK_MEM_SIZE);
    mmu.Map(STACK_REGION_BASE, stack_mem,
            {MemoryType::Stack, MemoryAttribute::None,
             MemoryPermission::ReadWrite});
    // TODO: correct? (I don't think so)
    mmu.Map(ALIAS_REGION_BASE, stack_mem,
            {MemoryType::Alias, MemoryAttribute::None,
             MemoryPermission::ReadWrite});
}

Process::~Process() {
    HW::TegraX1::CPU::MMUBase::GetInstance().Unmap(STACK_REGION_BASE,
                                                   stack_mem->GetSize());
    HW::TegraX1::CPU::MMUBase::GetInstance().FreeMemory(stack_mem);
}

void Process::Run() { main_thread.handle->Run(); }

} // namespace Hydra::Horizon::Kernel
