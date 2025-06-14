#include "core/horizon/kernel/thread.hpp"

#include "core/debugger/debugger.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace hydra::horizon::kernel {

Thread::Thread(vaddr_t stack_top_addr_, i32 priority_)
    : stack_top_addr{stack_top_addr_}, priority{priority_} {
    tls_mem = KERNEL_INSTANCE.CreateTlsMemory(tls_addr);
}

Thread::~Thread() {
    if (t) {
        t->join();
        delete t;
    }

    hw::tegra_x1::cpu::MMUBase::GetInstance().Unmap(tls_addr,
                                                    tls_mem->GetSize());
    hw::tegra_x1::cpu::MMUBase::GetInstance().FreeMemory(tls_mem);
}

void Thread::Run() {
    ASSERT(entry_point != 0x0, Kernel, "Invalid entry point");

    t = new std::thread([&]() {
        hw::tegra_x1::cpu::ThreadBase* thread =
            hw::tegra_x1::cpu::CPUBase::GetInstance().CreateThread(tls_mem);

        DEBUGGER_INSTANCE.RegisterThisThread("Guest",
                                             thread); // TODO: handle ID?

        thread->Initialize(
            [this](hw::tegra_x1::cpu::ThreadBase* thread, u64 id) {
                return KERNEL_INSTANCE.SupervisorCall(this, thread, id);
            },
            tls_addr, stack_top_addr);

        thread->SetPC(entry_point);
        for (u32 i = 0; i < sizeof_array(args); i++)
            thread->SetRegX(i, args[i]);

        thread->Run();

        DEBUGGER_INSTANCE.UnregisterThisThread();

        delete thread;
    });
}

} // namespace hydra::horizon::kernel
