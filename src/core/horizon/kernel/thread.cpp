#include "core/horizon/kernel/thread.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon::Kernel {

Thread::Thread(vaddr_t stack_top_addr_, i32 priority_)
    : stack_top_addr{stack_top_addr_}, priority{priority_} {
    tls_mem = Kernel::GetInstance().CreateTlsMemory(tls_addr);
}

Thread::~Thread() {
    if (t) {
        t->join();
        delete t;
    }

    HW::TegraX1::CPU::MMUBase::GetInstance().Unmap(tls_addr,
                                                   tls_mem->GetSize());
    HW::TegraX1::CPU::MMUBase::GetInstance().FreeMemory(tls_mem);
}

void Thread::Run() {
    ASSERT(entry_point != 0x0, HorizonKernel, "Invalid entry point");

    t = new std::thread([&]() {
        HW::TegraX1::CPU::ThreadBase* thread =
            HW::TegraX1::CPU::CPUBase::GetInstance().CreateThread(tls_mem);
        thread->Initialize(
            [](HW::TegraX1::CPU::ThreadBase* thread, u64 id) {
                return Kernel::GetInstance().SupervisorCall(thread, id);
            },
            tls_addr, stack_top_addr);

        thread->SetRegPC(entry_point);
        for (u32 i = 0; i < sizeof_array(args); i++)
            thread->SetRegX(i, args[i]);

        thread->Run();

        delete thread;
    });
}

} // namespace Hydra::Horizon::Kernel
